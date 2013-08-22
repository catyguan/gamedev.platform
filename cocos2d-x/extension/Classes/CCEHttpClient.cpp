#include "CCEHttpClient.h"
#include "curl.h"
#include "CCEConfig.h"
#include "CCEUtil.h"
#include "cocoa\CCValueSupport.h"

USING_NS_CC;

// CCEHttpClient
CCEHttpClient g_sharedHttpClient;
CCEHttpClient::CCEHttpClient()
{
	m_globalInit = false;
	m_handle = NULL;
	m_idSeq = 0;
}

CCEHttpClient::~CCEHttpClient()
{	
	
}

void CCEHttpClient::delreq(HttpClientReq* req)
{
	if(req!=NULL) {
		if(req->ch) {
			if(req->added && m_handle!=NULL) {
				curl_multi_remove_handle((CURLM*) m_handle, req->ch);
			}
			curl_easy_cleanup(req->ch);
		}
		if(req->data!=NULL) {
			delete[] req->data;
		}
		if(req->headerList!=NULL) {
			curl_slist_free_all(req->headerList);
		}
		req->callback.cleanup();
		delete req;
	}
}


void CCEHttpClient::cleanup()
{
	if(m_reqs.size()>0) {
		std::vector<HttpClientReq*>::const_iterator it = m_reqs.begin();
		for(;it!=m_reqs.end();it++) {
			delreq(*it);
		}
		m_reqs.clear();
	}
	if(m_handle!=NULL) {
		curl_multi_cleanup((CURLM*) m_handle);
		m_handle = NULL;
	}
	if(m_globalInit) {
		m_globalInit = false;
		curl_global_cleanup();
	}
}

CCEHttpClient* CCEHttpClient::sharedHttpClient()
{	
	return &g_sharedHttpClient;
}

void CCEHttpClient::purgeSharedHttpClient()
{
	g_sharedHttpClient.cleanup();
}

void CCEHttpClient::appRunnable(void* data, long mstick)
{
	CCEHttpClient* cl = (CCEHttpClient*) data;
	if(cl!=NULL) {
		if(cl->m_handle!=NULL && cl->m_reqs.size()>0) {
			CURLM* mh = (CURLM*) cl->m_handle;
			int running = 0;
			CURLMcode code = curl_multi_perform(mh, &running);
			if(code==CURLM_CALL_MULTI_PERFORM) {
				return;
			}
			if (code != CURLM_OK) {
				CCLOG("httpclient perform fail, code[%d]msg[%s]", code, curl_multi_strerror(code));
				return;
			}
			// CCLOG("httpclient performing(%d)", running);
			int mcount = 0;
			CURLMsg* msg = curl_multi_info_read(mh, &mcount);
			while(msg!=NULL) {
				if(msg->msg==CURLMSG_DONE) {
					HttpClientReq* req = NULL;
					std::vector<HttpClientReq*>::const_iterator it = cl->m_reqs.begin();
					for(;it!=cl->m_reqs.end();it++) {
						if((*it)->ch==msg->easy_handle) {
							req = *it;
							cl->m_reqs.erase(it);							
							break;
						}
					}

					CC_ASSERT(req);

					CURLcode result = msg->data.result;
					CCValueMap rep;
					rep["id"] = CCValue::intValue(req->id);
					rep["url"] = CCValue::stringValue(req->url);
					if(result!=0) {
						rep["result"] = CCValue::booleanValue(false);
						rep["errno"] = CCValue::intValue(result);
						rep["message"] = CCValue::stringValue(curl_easy_strerror(result));
					} else {											
						rep["result"] = CCValue::booleanValue(true);
						int code;
						curl_easy_getinfo(req->ch, CURLINFO_RESPONSE_CODE, &code);
						rep["status"] = CCValue::intValue(code);
						std::string content(req->data, req->size);
						rep["content"] = CCValue::stringValue(content);

						CCValueArray headers;
						std::vector<std::string>::const_iterator it2 = req->headers.begin();
						for(;it2!=req->headers.end();it2++) {
							headers.push_back(CCValue::stringValue(*it2));
						}
						rep["headers"] = CCValue::arrayValue(headers);
					}

					CCValue cb = req->callback;
					cb.retain();
					cl->delreq(req);
					if(cb.canCall()) {
						CCValueArray ps;
						ps.push_back(CCValue::mapValue(rep));
						cb.call(ps, false);
					}
				}
				msg = curl_multi_info_read(mh, &mcount);
			}
		}
	}
}

int httpclient_debug_callback(CURL* ch, curl_infotype type, char* data, size_t size, void* ptr)
{
	HttpClientReq* req = (HttpClientReq*) ptr;	
	std::string header;
	switch(type)
	{
	case CURLINFO_TEXT:
		header = "      text";		
		break;
	case CURLINFO_HEADER_IN:
		header = " header-in";		
		break;
	case CURLINFO_HEADER_OUT:
		header = " header-out";
		break;
	case CURLINFO_DATA_IN:
		header = "    data-in";		
		break;
	case CURLINFO_DATA_OUT:
		header = "   data-out";		
		break;
	}	

	if(type==CURLINFO_DATA_IN) {
		CCLOG("HTTPCLIENT(%d) %s: [%d]", req->id, header.c_str(), size);
	} else {
		std::string str(data, size>1?size-1:0);
		CCLOG("HTTPCLIENT(%d) %s: %s", req->id, header.c_str(), str.c_str());		
	}
	return 0;
}

size_t httpclient_headerFunction( void *ptr, size_t size, size_t nmemb, void *userdata)
{
	HttpClientReq* req = (HttpClientReq*) userdata;

	std::string header((char*) ptr, size*nmemb);
	req->headers.push_back(header);

	return size * nmemb; 
}

size_t httpclient_writeFunction( void *ptr, size_t size, size_t nmemb, void *userdata)
{
	HttpClientReq* req = (HttpClientReq*) userdata;
	size_t sz = size*nmemb;
	size_t gs = req->datasize;
	while(gs<req->size+sz) {
		gs += (req->bufferSize==0?1024*4:req->bufferSize);
	}
	if(gs>req->datasize) {
		char* tmp = new char[gs];
		if(req->data!=NULL) {
			memcpy(tmp, req->data, req->size);
			delete[] req->data;
		}
		req->data = tmp;
		req->datasize = gs;
	}
	memcpy(req->data, ptr, sz);
	req->size+=sz;
	return sz; 
}

int CCEHttpClient::process(CCValue& reqData, CCValue callback)
{
	if(!m_globalInit) {
		curl_global_init(CURL_GLOBAL_ALL);
		m_globalInit = true;
	}

	m_idSeq++;
	if(m_idSeq>1000000) {
		m_idSeq = 1;
	}
	int id = m_idSeq;

	HttpClientReq* req = create(id, reqData);
	req->callback = callback;
	req->callback.retain();

	if(m_handle==NULL) {
		m_handle = curl_multi_init();
		if(m_handle==NULL) {
			delreq(req);
			throw new std::string("curl_multi_init fail");
		}
	}
	CURLM* mh = (CURLM*) m_handle;
	CURLMcode mcode = curl_multi_add_handle(mh, req->ch); 
	if(mcode!=0) {
		delreq(req);
		throw new std::string("curl_multi_add_handle fail");
	}
	req->added = true;
	m_reqs.push_back(req);

	return id;
}

bool CCEHttpClient::cancel(int reqId)
{
	HttpClientReq* req = NULL;
	std::vector<HttpClientReq*>::const_iterator it = m_reqs.begin();
	for(;it!=m_reqs.end();it++) {
		if((*it)->id==reqId) {
			req = (*it);
			m_reqs.erase(it);
			break;
		}
	}
	if(req!=NULL) {
		delreq(req);
		return true;
	}
	return false;
}

HttpClientReq* CCEHttpClient::create(int reqId, CCValue& reqData)
{
	CCValueReader vr(&reqData);
	std::string url;
	url = vr.get("url")->stringValue();
	if(url.size()==0) {
		throw new std::string("invalid httpclient request,expect 'url'");
	}

	HttpClientReq* req = new HttpClientReq();
	req->id = reqId;
	req->url = url;
	req->bufferSize = vr.get("bufferSize")->intValue();
	req->ch = NULL;
	req->data = NULL;
	req->datasize = 0;
	req->size = 0;
	req->added = false;
	req->headerList = NULL;

	req->ch = curl_easy_init();
	if(!req->ch) {
		delete req;
		throw new std::string("curl_easy_init fail");
	}
	CURL* ch = req->ch;
	std::string ps;
	if(true) {
		CCValue* v = vr.getNull("params");
		if(v!=NULL) {
			if(v->isMap()) {
				CCValueMap* map = v->mapValue();
				CCValueMapIterator it = map->begin();
				for(;it!=map->end();it++) {
					if(ps.size()>0)ps += "&";
					char* tmpv = curl_easy_escape(ch, CCValueUtil::toString((CCValue&) (it->second)).c_str(),0);
					ps += it->first + "=" + std::string(tmpv);
					curl_free(tmpv);
				}
			} else {
				ps = v->stringValue();
			}
		}
	}
	std::string pfield;
	if(true) {
		CCValue* v = vr.getNull("postfield");
		if(v!=NULL) {
			if(v->isMap()) {
				CCValueMap* map = v->mapValue();
				CCValueMapIterator it = map->begin();
				for(;it!=map->end();it++) {
					if(pfield.size()>0)pfield += "&";
					char* tmpv = curl_easy_escape(ch, CCValueUtil::toString((CCValue&) (it->second)).c_str(),0);
					pfield += it->first + "=" + std::string(tmpv);
					curl_free(tmpv);
				}
			} else {
				pfield = v->stringValue();
			}
		}
	}

	bool postMode = vr.get("postMode")->booleanValue();
	std::string rurl = url;
	if(ps.size()>0 && !postMode) {
		if(rurl.find('?')==rurl.npos) {
			rurl += "?";
		} else {
			rurl += "&";
		}
		rurl+=ps;
	}

	curl_easy_setopt(ch, CURLOPT_URL, rurl.c_str());
	if(CCEConfig::get("httpclient_debug").booleanValue() || vr.get("debug")->booleanValue()) {
		curl_easy_setopt(ch, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(ch, CURLOPT_DEBUGFUNCTION, httpclient_debug_callback);
		curl_easy_setopt(ch, CURLOPT_DEBUGDATA, req);
	}
	curl_easy_setopt(ch, CURLOPT_HEADERFUNCTION, httpclient_headerFunction);
	curl_easy_setopt(ch, CURLOPT_WRITEHEADER, req);
	curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, httpclient_writeFunction);
	curl_easy_setopt(ch, CURLOPT_WRITEDATA, req);
	if(true) {
		int timeout = vr.get("timeout")->intValue();
		if(timeout<=0) {
			timeout = 30;
		}
		curl_easy_setopt(ch, CURLOPT_TIMEOUT, timeout);
	}
	// not sure about below
	// curl_easy_setopt(ch, CURLOPT_NOSIGNAL, 1);
	if(vr.get("redirectMode")->booleanValue()) {
		curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 5);
	}
	if(true) {
		CCValue* hvalue = vr.getNull("headers");
		if(hvalue!=NULL && hvalue->isMap()) {
			CCValueMap* hmap = hvalue->mapValue();
			CCValueMapIterator it = hmap->begin();
			for(;it!=hmap->end();it++) {
				if(it->first.compare("User-Agent")==0) {
					std::string hv = it->second.stringValue();
					if(hv.size()>0) {
						curl_easy_setopt(ch, CURLOPT_USERAGENT, hv.c_str());
					}
				} else if(it->first.compare("Cookie")==0) {
					std::string cookieValue;
					if(it->second.isMap()) {
						CCValueMap* ckmap = it->second.mapValue();
						CCValueMapIterator it2 = ckmap->begin();
						for(;it2!=ckmap->end();it2++) {
							if(cookieValue.size()>0)cookieValue += "; ";
							char* tmpv = curl_easy_escape(ch, CCValueUtil::toString((CCValue&) (it2->second)).c_str(),0);
							cookieValue += it2->first + "=" + std::string(tmpv);
							curl_free(tmpv);
						}
					} else {
						cookieValue = CCValueUtil::toString((CCValue&) (it->second));
					}
					curl_easy_setopt(ch, CURLOPT_COOKIE, cookieValue.c_str());
				} else if(it->first.compare("Referrer")==0) {
					std::string tmpv = it->second.stringValue();
					if(tmpv.size()>0) {
						curl_easy_setopt(ch, CURLOPT_REFERER, tmpv.c_str());
					}
				} else {
					char* tmpv = curl_easy_escape(ch, CCValueUtil::toString((CCValue&) (it->second)).c_str(),0);
					std::string tmpv2 = it->first + ": " + std::string(tmpv);
					curl_free(tmpv);
					req->headerList = curl_slist_append(req->headerList, tmpv2.c_str());
				}
			}
			if(req->headerList!=NULL) {
				curl_easy_setopt(ch, CURLOPT_HTTPHEADER, req->headerList);
			}
		}
	}

	if(true) {
		if(postMode || pfield.size()>0) {
			curl_easy_setopt(ch, CURLOPT_POST, 1);
			std::string* data;
			if(pfield.size()>0) {
				data = &pfield;
			} else {
				data = &ps;
			}
			curl_easy_setopt(ch, CURLOPT_POSTFIELDSIZE, data->size());
			curl_easy_setopt(ch, CURLOPT_COPYPOSTFIELDS, data->c_str());
		}
	}

	return req;
}

int CCEHttpClient::queryRunningCount()
{
	return m_reqs.size();
}

std::string CCEHttpClient::escape(std::string v)
{
	CURL* ch = curl_easy_init();
	char* str = curl_easy_escape(ch, v.c_str(), v.size());
	std::string r(str);
	curl_free(str);
	curl_easy_cleanup(ch);
	return r;
}
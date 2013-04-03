#include "CCEThriftRemote.h"

#include "CCEAppUtil.h"
#include "CCEThriftRemote_impl.h"
#include "../thrift/TApplicationException.h"
#include "../thrift/transport/TMemoryTransports.h"
#include "../thrift/protocol/TBinaryProtocol.h"
#include "../thrift/protocol/TScheme.h"

USING_NS_CC;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

// CCEThriftRemote
CCEThriftRemote* s_pSharedRemote;

void CCEThriftRemote::install()
{
    CC_ASSERT(s_pSharedRemote==NULL);
	s_pSharedRemote = this;
	start();
}

CCEThriftRemote* CCEThriftRemote::sharedRemote()
{
	return s_pSharedRemote;
}

void CCEThriftRemote::purgeSharedRemote()
{
	if (s_pSharedRemote)
    {
		s_pSharedRemote->cleanup();
		delete s_pSharedRemote;
    }
}

CCEThriftRemote::CCEThriftRemote()
: m_retryConnectTime(3000)
, m_lastConnectTime(0)
, m_frameMaxLength(1024*1024)
, m_gateIndex(0)
, m_service(NULL)
{
	
}

CCEThriftRemote::~CCEThriftRemote()
{
	if(m_service!=NULL) {
		delete m_service;
	}
}

void CCEThriftRemote::addGate(const char* hostname, int p)
{
	m_gateList.push_back(std::pair<std::string,int>(hostname,p));
}

void CCEThriftRemote::clearGate()
{
	m_gateList.clear();
	m_gateIndex = 0;
}

void CCEThriftRemote::setRetryConnectTime(int time)
{
	m_retryConnectTime = time;
}

void CCEThriftRemote::start()
{
	m_socket.start();
}

void CCEThriftRemote::cleanup()
{
	m_socket.stop();
}

bool CCEThriftRemote::isConnected()
{
	return m_socket.isOpen();
}

bool CCEThriftRemote::onewayCall(std::string name, CCValue& msg)
{
	int sid = call(name,msg,NULL,0);
	if(sid>0) {
		discardCall(sid);
		return true;
	}
	return false;
}

std::pair<std::string, int>* CCEThriftRemote::currentGate()
{
	if(m_gateIndex<m_gateList.size()) {
		return &(m_gateList[m_gateIndex]);
	}
	return NULL;
}

void CCEThriftRemote::nextGate()
{
	m_gateIndex++;
	if(m_gateIndex<m_gateList.size())return;
	m_gateIndex = 0;
}

void CCEThriftRemote::appRunnable(void* data, long mstick)
{
	CCEThriftRemote* p = (CCEThriftRemote*) data;
	CCEAsynSocket::appRunnable(&p->m_socket, mstick);
}

// CCEThriftResponseHandlerSimple
CCEThriftResponseHandlerSimple::CCEThriftResponseHandlerSimple(CCValue* result, CCValue* error, CCValue* timeout)
{
	if(result!=NULL) {
		resultCall = *result;
		resultCall.retain();
	}
	if(error!=NULL) {
		errorCall = *error;
		errorCall.retain();
		timeoutCall = *error;
	}
	if(timeout!=NULL) {
		timeoutCall = *timeout;
		timeoutCall.retain();
	}
}

CCEThriftResponseHandlerSimple::~CCEThriftResponseHandlerSimple()
{

}

bool CCEThriftResponseHandlerSimple::onTimeout()
{
	if(timeoutCall.canCall()) {
		CCValueArray ps;
		ps.push_back(CCValue::stringValue("timeout"));
		timeoutCall.call(ps, false);
	}
	return true;
}

bool CCEThriftResponseHandlerSimple::handleResponse(CCValueArray& msg)
{
	if(resultCall.canCall()) {
		resultCall.call(msg,false);	
	}
	return true;
}
bool CCEThriftResponseHandlerSimple::handleError(CCValue& err)
{
	if(errorCall.canCall()) {
		CCValueArray ps;
		ps.push_back(err);
		errorCall.call(ps, false);
	}
	return true;
}

// CCEThriftRemoteServiceSimple
CCEThriftRemoteServiceSimple::CCEThriftRemoteServiceSimple(CCValue* c)
{
	if(c!=NULL) {
		call = *c;
		call.retain();
	}
}

CCEThriftRemoteServiceSimple::~CCEThriftRemoteServiceSimple()
{

}

bool CCEThriftRemoteServiceSimple::handleRequest(int seqid, std::string module, std::string name, int type, CCValueBuilder* msg)
{	
	if(call.canCall()) {
		CCValueArray ps;
		ps.push_back(CCValue::intValue(seqid));
		ps.push_back(CCValue::stringValue(module));
		ps.push_back(CCValue::stringValue(name));
		ps.push_back(CCValue::intValue(type));
		msg->build(&ps);
		call.call(ps, false);
	}
	return true;
}

// CCEThriftRemoteCall
CCEThriftRemoteCall::~CCEThriftRemoteCall() {
	if(handler!=NULL && !waiting) {
		delete handler;
	}
}

// CCEThriftRemoteSimple
CCEThriftRemoteSimple::CCEThriftRemoteSimple()
	: m_seqId(0)
	, m_headerPos(0)
	, m_dataPos(0)
	, m_dataLen(0)
	, m_data(NULL)
	, m_dataSize(0)
	, m_connecting(false)
{

}

CCEThriftRemoteSimple::~CCEThriftRemoteSimple()
{

}
	
void CCEThriftRemoteSimple::resetBuffer()
{
	m_headerPos = 0;
	m_dataPos = 0;
	m_dataLen = 0;
}

void CCEThriftRemoteSimple::start()
{
	m_socket.setHandler(this);
	CCEThriftRemote::start();
}

void CCEThriftRemoteSimple::cleanup()
{
	if(true) {
		std::list<CCEThriftRemoteCall*>::const_iterator it = m_await.begin();
		while(it!=m_await.end()) {
			CCEThriftRemoteCall* call = *it;
			delete call;
			it++;			
		}
		m_await.clear();
	}
	if(true) {
		std::map<int,ThriftRemoteResponseHandler>::const_iterator it = m_callbacks.begin();
		while(it!=m_callbacks.end()) {
			if(it->second.handler!=NULL) {
				delete it->second.handler;
			}
			it++;		
		}
		m_callbacks.clear();
	}
	CCEThriftRemote::cleanup();
	if(m_data!=NULL) {
		delete m_data;
		m_data = NULL;
		resetBuffer();
	}
}

bool CCEThriftRemoteSimple::handleUpstream(CCEAsynSocketEvent* ev)
{
	if(true) {
		CCEAsynSocketEventOpen* evt = dynamic_cast<CCEAsynSocketEventOpen*>(ev);
		if(evt!=NULL) {
			std::pair<std::string,int>* gateInfo = currentGate();			
			if(evt->isOpen()) {
				CCLOG("remote[%s:%d] connected", gateInfo->first.c_str(), gateInfo->second);
				m_connecting = false;
				resetBuffer();

				// send all await calls
				if(m_await.size()>0) {
					CCLOG("send await calls - %d", m_await.size());
					std::list<CCEThriftRemoteCall*>::const_iterator it;
					while((it=m_await.begin())!=m_await.end()) {
						CCEThriftRemoteCall* call = *it;
						m_await.erase(it);
						send(call);
						delete call;					
					}
				}

				onRemoteConnected();

			} else {
				CCLOG("remote[%s:%d] closed %s", gateInfo->first.c_str(), gateInfo->second, evt->debugString().c_str());
				if(evt->isConnect()) {
					nextGate();
				}
				if(m_retryConnectTime>0) {						
					CCLOG("wait %d to reconnect", m_retryConnectTime);
					m_socket.addTimeout("reconnect", NULL, m_retryConnectTime);
				} else {
					m_connecting = false;
				}				

				onRemoteClose();
			}
			return true;
		}
	}
	if(true) {
		CCEAsynSocketEventRead* evt = dynamic_cast<CCEAsynSocketEventRead*>(ev);
		if(evt!=NULL) {
			const char* buf = evt->getData();
			int len = evt->getSize();
			while(len>0) {
				int rd = handleData(buf,len);
				if(rd<=0)break;
				len-=rd;
				buf += rd;
			}
		}
	}
	if(true) {
		CCEAsynSocketEventTimeout* evt = dynamic_cast<CCEAsynSocketEventTimeout*>(ev);
		if(evt!=NULL) {
			if(evt->isName("reconnect")) {
				checkConnect(true);
			}
			return true;
		}
	}

	return true;
}

void CCEThriftRemoteSimple::onRemoteConnected()
{

}

void CCEThriftRemoteSimple::onRemoteClose()
{

}

bool CCEThriftRemoteSimple::checkConnect(bool f)
{
	if(isConnected()) {
		return true;
	}
	if (m_connecting) {
		if(!f && m_retryConnectTime>0) {
			long now = CCEAppUtil::timeTick();
			if(now - m_lastConnectTime < m_retryConnectTime) {
				return false;
			}		
		}
		
	}	

	m_connecting = true;
	m_lastConnectTime = CCEAppUtil::timeTick();

	std::pair<std::string, int>* gateInfo = currentGate();
	if(gateInfo==NULL) {
		CCLOG("connect fail, no gate info");
		return false;
	}
	std::string host = gateInfo->first;
	int port = gateInfo->second;

	CCLOG("connecting %s : %d",host.c_str(), port);
	m_socket.open(host.c_str(), port);
	return true;
}
	
void CCEThriftRemoteSimple::split(std::string& target, std::string& module, std::string& name)
{
	size_t last = 0;
	size_t index= target.find_first_of('.',last);
	if(index!=std::string::npos)
	{
		module = target.substr(last,index-last);
		last=index+1;			
		name = target.substr(last,index-last);
	} else {
		name = target;
	}
}

int CCEThriftRemoteSimple::handleData(const char* data,int len)
{
	// frame
	int rd = 0;
	if(m_headerPos<4) {
		int r = min(4-m_headerPos, len);
		uint8_t* szp = reinterpret_cast<uint8_t*>(&m_header) + m_headerPos;		
		memcpy(szp, data, r);		
		m_headerPos+=r;
		rd+=r;
		if(m_headerPos<4)return rd;
		m_dataPos = 0;
		m_dataLen = ntohl(m_header);
		if(m_dataLen>m_frameMaxLength) {
			CCLOG("frame size too large %d/%d", m_dataLen, m_frameMaxLength);
			m_socket.close();
			return -1;
		}
		data+=r;
		len-=r;
	}	

	if(m_dataSize<m_dataLen) {
		if(m_data!=NULL)delete m_data;
		m_data = new char[m_dataLen];
		m_dataSize = m_dataLen;
	}
	int r = min(m_dataLen-m_dataPos, len);
	if(r>0) {
		memcpy(m_data+m_dataPos, data, r);
		m_dataPos+=r;
		rd+=r;
	}

	if(m_dataPos<m_dataLen) {
		return rd;
	}

	std::string target;
	TMessageType type = TMessageType::T_EXCEPTION;
	int32_t seqid = 0;
	try {
		TMemoryBuffer mtrans((uint8_t*) m_data, m_dataLen);
		TBinaryProtocol binp(&mtrans);
		TProtocol* in = &binp;

		in->readMessageBegin(target,type,seqid);

#if COCOS2D_DEBUG == 1
		if(true) {
			CCLOG("TMessage - %d, %d, %s",seqid, type, target.c_str());
		}
#endif

		if(type==TMessageType::T_CALL || type==TMessageType::T_ONEWAY) {
			// request
			std::string module;
			std::string name;

			split(target, module, name);

			TScheme* sch = TSchemeService::findScheme(module, name, SCHEME_TYPE_ARGS);
			if(sch==NULL) {
				::apache::thrift::TApplicationException ex("unknow module: "+module);
				thriftSend(seqid, target, &ex);
				resetBuffer();
				return rd;
			}
			CCValueBuilder vb;
			sch->read(in, &vb);
			in->readMessageEnd();
			resetBuffer();
			
			if(m_service!=NULL) {
				m_service->handleRequest(seqid,module,name,type,&vb);
			}

			return rd;				
		}
		
		if(type==TMessageType::T_REPLY) {
			// response
			std::map<int, ThriftRemoteResponseHandler>::const_iterator it = m_callbacks.find(seqid);
			if(it==m_callbacks.end()) {
				CCLOG("skip response [%d]", seqid);				
			} else {
				CCValueBuilder vb;
				if(it->second.scheme!=NULL) {
					it->second.scheme->read(in, &vb);
				}			
				in->readMessageEnd();
				resetBuffer();
			
				CCEThriftResponseHandler* handler = it->second.handler;
				m_callbacks.erase(it);
				if(handler!=NULL) {
					CCValueArray ps;
					vb.build(&ps);
					handler->handleResponse(ps);
					delete handler;
				}
				
			}

			return rd;					
		}

		if(type==TMessageType::T_EXCEPTION) {
			// error
			std::map<int, ThriftRemoteResponseHandler>::const_iterator it = m_callbacks.find(seqid);
			if(it==m_callbacks.end()) {
				CCLOG("skip response [%d]", seqid);				
			} else {
				TApplicationException ex;
				ex.read(in);
				in->readMessageEnd();
				resetBuffer();

				CCValueMap map;
				map["type"] = CCValue::intValue(ex.getType());
				map["what"] = CCValue::stringValue(ex.what());
			
				CCEThriftResponseHandler* handler = it->second.handler;
				m_callbacks.erase(it);
				if(handler!=NULL) {
					CCValue v = CCValue::mapValue(map);
					handler->handleError(v);
					delete handler;
				}				
			}
			
			return rd;					
		}
		
	} catch (TException &tx) {
		CCLOG("handleData exception %s", tx.what());
		if(type==TMessageType::T_CALL) {
			TApplicationException ex(TApplicationException::INTERNAL_ERROR, tx.what());
			thriftSend(seqid, target, &ex);
		}
	}  

	resetBuffer();
	return rd;
}

void CCEThriftRemoteSimple::thriftSend(int seqid, std::string name, ::apache::thrift::TApplicationException* ex)
{
	TMemoryBuffer mtrans(128);
	TBinaryProtocol binp(&mtrans);
	TProtocol* out = &binp;

	// TFrame
	out->writeI32(0);

	// TMessage
	out->writeMessageBegin(name,TMessageType::T_EXCEPTION,seqid);
	ex->write(out);	
	out->writeMessageEnd();

	uint8_t* data;
	uint32_t sz;
	mtrans.getBuffer(&data, &sz);
	
	uint32_t len = sz-4;
	*((uint32_t*) data) = (uint32_t)htonl((uint32_t)(len));	
	m_socket.write(seqid, (char*) data, sz);
}

void CCEThriftRemoteSimple::thriftSend(int seqid, std::string name, 
		::apache::thrift::protocol::TMessageType type, TScheme* msg, CCValue* value)
{
	TMemoryBuffer mtrans(128);
	TBinaryProtocol binp(&mtrans);
	TProtocol* out = &binp;

	// TFrame
	out->writeI32(0);

	// TMessage
	out->writeMessageBegin(name,type,seqid);
	if(msg!=NULL) {
		msg->write(out, value);
	}
	out->writeMessageEnd();

	uint8_t* data;
	uint32_t sz;
	mtrans.getBuffer(&data, &sz);
	
	uint32_t len = sz-4;
	*((uint32_t*) data) = (uint32_t)htonl((uint32_t)(len));	
	m_socket.write(type==TMessageType::T_CALL?seqid:0, (char*) data, sz);
}

int CCEThriftRemoteSimple::call(std::string target,CCValue& value, CCEThriftResponseHandler* handler, int timeout)
{
	std::string module;
	std::string name;
	split(target,module,name);

	TScheme* args = TSchemeService::findScheme(module,name,SCHEME_TYPE_ARGS);
	TScheme* rets = TSchemeService::findScheme(module,name,SCHEME_TYPE_RETURN);
	CC_ASSERT(args!=NULL);
	CC_ASSERT(rets!=NULL);
	if(args==NULL || rets==NULL) {		
		if(handler!=NULL) {
			delete handler;
		}
		return -1;
	}

	m_seqId++;
	if(m_seqId>10000000) {
		m_seqId = 1;
	}
	CCEThriftRemoteCall* call = new CCEThriftRemoteCall();
	call->seqId = m_seqId;
	call->handler = handler;
	call->argScheme = args;
	call->retScheme = rets;
	call->value = value;
	call->name = target;
	call->type = handler==NULL?TMessageType::T_ONEWAY:TMessageType::T_CALL;	
	call->waiting = false;
	if(isConnected()) {
		send(call);
		delete call;
	} else {
		m_await.push_back(call);
	}
	if(timeout>0) {
		m_socket.addTimeout("call", (void*) m_seqId, timeout);
	}
	return m_seqId;
}

void CCEThriftRemoteSimple::discardCall(int callId)
{
	std::list<CCEThriftRemoteCall*>::const_iterator it = m_await.begin();
	while(it!=m_await.end()) {
		CCEThriftRemoteCall* call = *it;
		if(call->seqId==callId) {
			delete call;
			m_await.erase(it);
			break;
		}		
		it++;
	}

	std::map<int, ThriftRemoteResponseHandler>::const_iterator it2 = m_callbacks.find(callId);
	if(it2!=m_callbacks.end()) {
		if(it2->second.handler!=NULL) {
			delete it2->second.handler;
		}
		m_callbacks.erase(it2);
	}
}

void CCEThriftRemoteSimple::send(CCEThriftRemoteCall* call)
{	
	ThriftRemoteResponseHandler h;
	h.scheme = call->retScheme;
	h.handler = call->handler;
	m_callbacks[call->seqId] = h;
	call->waiting = true;
	thriftSend(call->seqId, call->name, call->type, call->argScheme, &call->value);
}
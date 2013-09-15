#include "CCEUpgradeManager.h"
#include "CCEHttpClient.h"
#include "platform/CCFileSystem.h"
#include "cocoa/CCValueSupport.h"
#include "CCEUtil.h"

USING_NS_CC;

// UpgradeDownloadObject
class UpgradeDownloadObject : public CCObject
{
public:
	virtual ~UpgradeDownloadObject(){};

public:
	static UpgradeDownloadObject* create(int type, int fstype, std::string fileName) {
		UpgradeDownloadObject* r = new UpgradeDownloadObject();
		r->m_type = type;
		r->m_fileName = fileName;
		r->m_filesystemType = fstype;
		r->autorelease();
		return r;
	};

	virtual CCValue invoke(CCValueArray& params) {
		CCEUpgradeManager::sharedUpgradeManager()->handleDownload(params, m_type, m_filesystemType, m_fileName);
		return CCValue::nullValue();
	};

protected:
	UpgradeDownloadObject(){};

	
	int m_type;
	int m_filesystemType;
	std::string m_fileName;
};

// CCEUpgradeManager
CCEUpgradeManager g_sharedManager;
CCEUpgradeManager::CCEUpgradeManager()
	: m_processer(NULL)
	, m_processerData(NULL)
	, m_processing(false)
	, m_httpclientRequestId(0)
{	
}

CCEUpgradeManager::~CCEUpgradeManager()
{	
	
}

CCEUpgradeManager* CCEUpgradeManager::sharedUpgradeManager(void)
{
	return &g_sharedManager;
}

void CCEUpgradeManager::getConfig(UpgradeConfig* cfg)
{
	std::string str = CCFileSystem::sharedFileSystem()->fileReadString(kAppData, "upgrade.ini");
	std::string delim("\n");
	std::vector<std::string> lines;

	StringUtil::split(str,delim,&lines);

	cfg->host = "";
	cfg->step = UPGRADE_STEP_QUERY;
	cfg->url = "";
	cfg->version = "";

	delim = "=";
	std::vector<std::string>::const_iterator it = lines.begin();
	for(;it!=lines.end();it++) {
		std::string ln = *it;
		ln = StringUtil::trim(ln);
		if(ln.size()==0)continue;

		std::vector<std::string> parts;
		StringUtil::split(ln, delim, &parts, 2);

		if(parts.size()!=2)continue;

		std::string key = parts[0];
		std::string val = parts[1];

		if(key.compare("url")==0) {
			cfg->url = val;
		} else if(key.compare("host")==0) {
			cfg->host = val;
		} else if(key.compare("step")==0) {
			cfg->step = atoi(val.c_str());
		} else if(key.compare("version")==0) {
			cfg->version = val;
		}
	}
}

bool CCEUpgradeManager::setConfig(UpgradeConfig* cfg)
{
	std::string content = "";
	content += StringUtil::format("url=%s\n",cfg->url.c_str());
	content += StringUtil::format("host=%s\n",cfg->host.c_str());
	content += StringUtil::format("step=%d\n", cfg->step);
	content += StringUtil::format("version=%s\n",cfg->version.c_str());
	return CCFileSystem::sharedFileSystem()->fileWriteString(kAppData, "upgrade.ini", content);
}

bool CCEUpgradeManager::isProcessing()
{
	return m_processing;
}
	
bool CCEUpgradeManager::start(CCEUpgradeProcesser processer,void *data)
{
	if(isProcessing())return false;

	m_processing = true;
	m_processer = processer;
	m_processerData = data;

	getConfig(&m_config);

	m_stepList.clear();
	doProcess();

	return true;
}

void CCEUpgradeManager::download(std::string url, std::string host, CCValueMap* params, int type, int fstype, std::string fileName)
{
	CCValueMap req;
		req["url"] = CCValue::stringValue(url);
		if(params!=NULL) {
			req["params"] = CCValue::mapValue(*params);
		}
		// req["debug"] = CCValue::booleanValue(true);
		CCValueMap headers;
		if(host.size()>0) {
			headers["Host"] = CCValue::stringValue(m_config.host);
			req["headers"] = CCValue::mapValue(headers);
		}

		CCValue reqv = CCValue::mapValue(req);
		UpgradeDownloadObject* obj = UpgradeDownloadObject::create(type, fstype, fileName);
		CCValue cb = CCValue::objectValue(obj);
		m_httpclientRequestId = CCEHttpClient::sharedHttpClient()->process(reqv, cb);
}

void CCEUpgradeManager::doProcess()
{
	while(_doProcess());
}

bool CCEUpgradeManager::_doProcess()
{
	while(m_stepList.size()>0) {
		std::vector<std::string>::iterator it = m_stepList.begin();
		std::string ln = *it;
		m_stepList.erase(it);

		ln = StringUtil::trim(ln);
		if(ln.size()==0) {
			callback(UPGRADE_STATUS_PROCESS, 100);
			continue;
		}

		std::vector<std::string> parts;
		StringUtil::split(ln, "=", &parts, 2);
		std::string cmd = parts[0];
		std::string param = parts.size()>1?parts[1]:"";
		if(cmd.compare("end")==0) {
			CCLOG("Upgrade: version %s, done", m_config.version.c_str());
			m_config.step = UPGRADE_STEP_QUERY;
			setConfig(&m_config);
			callback(UPGRADE_STATUS_END, 100);
			return false;
		}
		if(cmd.compare("dl")==0) {
			parts.clear();
			StringUtil::split(param,",",&parts, 4);
			if(parts.size()==4) {
				std::string strType = parts[0];
				int fstype = atoi(strType.c_str());
				std::string fileName = parts[1];
				std::string host = parts[2];
				std::string url = parts[3];
				CCLOG("Upgrade: download %d,%s from %s ...", fstype, fileName.c_str(), url.c_str());
				download(url,host,NULL,2, fstype, fileName);
				return false;
			}
		}
		if(cmd.compare("rm")==0) {
			parts.clear();
			StringUtil::split(param,",",&parts, 2);
			if(parts.size()==2) {
				std::string strType = parts[0];
				int fstype = atoi(strType.c_str());
				std::string fileName = parts[1];
				CCLOG("Upgrade: remove %d,%s", fstype, fileName.c_str());
				CCFileSystem::sharedFileSystem()->fileDelete(cocos2d::FileSystemType(fstype), fileName.c_str());
				return true;
			}
		}
		if(cmd.compare("version")==0) {
			parts.clear();
			StringUtil::split(param,",",&parts, 3);
			if(parts.size()==3) {
				m_config.version = parts[0];
				std::string host = parts[1];
				std::string url = parts[2];
				CCLOG("Upgrade: processing version %s ...", m_config.version.c_str());
				download(url,host,NULL,1, kAppData, "upgrade_"+m_config.version+".dat");
				return false;
			}
		}

		callback(UPGRADE_STATUS_PROCESS, 100);
	}

	// query
	if(m_config.step==UPGRADE_STEP_QUERY) {
		CCValueMap params;
		params["version"] = CCValue::stringValue(m_config.version);
		download(m_config.url, m_config.host, &params, 0, kAppData, "upgrade_query.dat");
		return false;
	}

	// load step
	if(true) {
		std::string name = "upgrade_"+m_config.version+".dat";
		std::string content = CCFileSystem::sharedFileSystem()->fileReadString(kAppData, name.c_str());
		if(content.size()==0) {
			CCLOG("Upgrade: version %s invalid, change to query", m_config.version.c_str());

			m_config.step = UPGRADE_STEP_QUERY;
			setConfig(&m_config);
			callback(UPGRADE_STATUS_END, 100);
			return false;
		} else {
			StringUtil::split(content, "\n", &m_stepList);
			CCLOG("Upgrade: loading version %s, %d", m_config.version.c_str(), m_stepList.size());
			return true;
		}
	}
}

void CCEUpgradeManager::callback(int status, float per)
{
	if(status!=UPGRADE_STATUS_PROCESS)m_processing = false;
	if(m_processer!=NULL) {
		m_processer(m_processerData, status, m_stepList.size(), per);
	}
}

void CCEUpgradeManager::handleDownload(CCValueArray& params,int type, int fstype, std::string fileName)
{
	if(params.size()==0) {
		return;
	}
	CCValueReader vr(&params[0]);
	if(!vr.get("result")->booleanValue()) {
		std::string msg = vr.get("message")->stringValue();
		CCLOG("Upgrade: download %d,%s fail - %s", fstype, fileName.c_str(), msg.c_str());
		callback(UPGRADE_STATUS_FAIL, 0);		
		return;
	}	
	
	if(vr.get("status")->intValue()!=200) {
		CCLOG("Upgrade: download %d,%s invalid response - %d", fstype, fileName.c_str(), vr.get("status")->intValue());
		callback(UPGRADE_STATUS_FAIL, 0);	
		return;
	}

	std::string content = vr.get("content")->stringValue();
	if(type==0) {
		// query
		std::string delim("\n");
		StringUtil::split(content, delim, &m_stepList);
		CCLOG("Upgrade: query version %s done", m_config.version.c_str());
	} else {
		if(!CCFileSystem::sharedFileSystem()->fileWriteString(cocos2d::FileSystemType(fstype), fileName.c_str(), content)) {
			CCLOG("Upgrade: download %d,%s save fail", fstype, fileName.c_str());
			callback(UPGRADE_STATUS_FAIL, 0);	
			return;
		}
		CCLOG("Upgrade: download %d,%s done", fstype, fileName.c_str(), vr.get("status")->intValue());
	}
	if(type==1) {
		m_config.step = UPGRADE_STEP_PROCESS;
		setConfig(&m_config);
	}
	callback(UPGRADE_STATUS_PROCESS, 100);
	doProcess();
}

bool CCEUpgradeManager::cancel()
{
	return false;
}

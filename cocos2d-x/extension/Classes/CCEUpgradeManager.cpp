#include "CCEUpgradeManager.h"
#include "CCEHttpClient.h"
#include "platform/CCFileSystem.h"
#include "cocoa/CCValueSupport.h"
#include "CCEUtil.h"
#include "CCEConfig.h"

USING_NS_CC;

// helper
std::string prop(std::map<std::string, std::string>& props, const char* name)
{
	std::map<std::string, std::string>::const_iterator it = props.find(name);
	if(it==props.end())return "";
	return it->second;
}

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
	: m_processing(false)
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
	std::map<std::string, std::string> props;
	StringUtil::parseProperties(str, "\n", "=", &props);

	cfg->host = CCEConfig::get("upgrade_host").stringValue();
	cfg->step = UPGRADE_STEP_QUERY;
	cfg->url = CCEConfig::get("upgrade_url").stringValue();
	cfg->version = CCEConfig::get("upgrade_version").stringValue();

	std::map<std::string, std::string>::const_iterator it = props.begin();
	for(;it!=props.end();it++) {
		std::string key = it->first;
		std::string val = it->second;

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
	
bool CCEUpgradeManager::start()
{
	if(isProcessing())return false;

	m_processing = true;

	getConfig(&m_config);

	m_info.end = false;
	m_info.fail = false;
	m_info.finishWork = 0;
	m_info.totalWork = 0;
	m_info.message = "";

	m_workList.clear();

	// query
	if(m_config.step==UPGRADE_STEP_QUERY) {
		std::string work("q:");
		work += StringUtil::format("l=%s", m_config.url.c_str());
		if(m_config.host.size()>0) {
			work += StringUtil::format(",h=%s", m_config.host.c_str());
		}
		if(m_config.version.size()>0) {
			work += StringUtil::format(",v=%s", m_config.version.c_str());
		}
		addWorks(work);
	} else {
		std::string work("lv:");
		if(m_config.version.size()>0) {
			work += StringUtil::format("v=%s", m_config.version.c_str());
		}
		addWorks(work);
	}

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

void CCEUpgradeManager::addWorks(std::string s)
{
	int c = m_workList.size();
	StringUtil::split(s, "\n", &m_workList);
	m_info.totalWork += m_workList.size()-c;
}

void CCEUpgradeManager::doProcess()
{
	while(_doProcess());
}

bool CCEUpgradeManager::_doProcess()
{
	std::map<std::string, std::string> props;
	while(m_workList.size()>0) {
		std::vector<std::string>::iterator it = m_workList.begin();
		std::string ln = *it;
		m_workList.erase(it);

		ln = StringUtil::trim(ln);
		if(ln.size()==0) {
			finishWork(UPGRADE_STATUS_PROCESS, NULL);
			continue;
		}

		std::vector<std::string> parts;
		StringUtil::split(ln, ":", &parts, 2);
		std::string cmd = parts[0];
		std::string param = parts.size()>1?parts[1]:"";
		if(cmd.compare("no")==0) {
			std::string msg("no upgrade");
			CCLOG("Upgrade: no upgrade", msg.c_str());
			finishWork(UPGRADE_STATUS_END, msg.c_str());
			return false;
		}
		if(cmd.compare("end")==0) {
			std::string msg = StringUtil::format("version %s done", m_config.version.c_str());
			CCLOG("Upgrade: %s", msg.c_str());
			m_config.step = UPGRADE_STEP_QUERY;
			setConfig(&m_config);
			finishWork(UPGRADE_STATUS_END, msg.c_str());
			return false;
		}
		if(cmd.compare("dl")==0) {
			props.clear();
			StringUtil::parseProperties(param,",","=", &props);			
			int fstype = atoi(prop(props, "t").c_str());
			std::string fileName = prop(props,"f");
			std::string host = prop(props,"h");
			std::string url = prop(props, "l");
			CCLOG("Upgrade: download %d,%s from %s ...", fstype, fileName.c_str(), url.c_str());
			download(url,host,NULL,2, fstype, fileName);
			return false;			
		}
		if(cmd.compare("rm")==0) {
			props.clear();
			StringUtil::parseProperties(param,",","=", &props);
			int fstype = atoi(prop(props, "t").c_str());
			std::string fileName = prop(props,"f");
			CCLOG("Upgrade: remove %d,%s", fstype, fileName.c_str());
			CCFileSystem::sharedFileSystem()->fileDelete(cocos2d::FileSystemType(fstype), fileName.c_str());
			finishWork(UPGRADE_STATUS_PROCESS, NULL);
			return true;			
		}
		if(cmd.compare("dv")==0) {
			props.clear();
			StringUtil::parseProperties(param,",","=", &props);
			std::string version = prop(props,"v");
			std::string host = prop(props,"h");
			std::string url = prop(props,"l");
			CCLOG("Upgrade: loading version %s ...", version.c_str());
			download(url,host,NULL,1, kAppData, "upgrade_"+version+".dat");
			return false;			
		}
		if(cmd.compare("q")==0) {
			props.clear();
			StringUtil::parseProperties(param,",","=", &props);
			std::string host = prop(props,"h");
			std::string url = prop(props,"l");
			std::string version = prop(props,"v");
			CCValueMap params;
			params["version"] = CCValue::stringValue(version);
			download(url, host, &params, 0, kAppData, "upgrade_query.dat");
			return false;			
		}
		if(cmd.compare("lv")==0) {
			props.clear();
			StringUtil::parseProperties(param,",","=", &props);
			std::string version = prop(props,"v");
			std::string name = "upgrade_"+version+".dat";
			std::string content = CCFileSystem::sharedFileSystem()->fileReadString(kAppData, name.c_str());
			if(content.size()==0) {
				std::string msg = StringUtil::format("load version %s fail", version.c_str());
				CCLOG("Upgrade: %s", msg.c_str());
				finishWork(UPGRADE_STATUS_END, msg.c_str());
				return true;
			} else {				
				m_config.version = version;
				m_config.step = UPGRADE_STEP_PROCESS;
				setConfig(&m_config);
				addWorks(content);
				CCLOG("Upgrade: loaded version %s", m_config.version.c_str());
				return true;
			}
		}

		CCLOG("Upgrade: unknow command %s!!!!", cmd.c_str());
		finishWork(UPGRADE_STATUS_PROCESS, NULL);
	}
	return false;
}

void CCEUpgradeManager::finishWork(int status, const char* msg)
{	
	if(status!=UPGRADE_STATUS_PROCESS) {
		m_processing = false;
		m_info.end = true;
		m_info.fail = status==UPGRADE_STATUS_FAIL;
	}
	if(m_info.finishWork<m_info.totalWork)m_info.finishWork++;
}

void CCEUpgradeManager::handleDownload(CCValueArray& params,int type, int fstype, std::string fileName)
{
	if(params.size()==0) {
		return;
	}
	CCValueReader vr(&params[0]);
	if(!vr.get("result")->booleanValue()) {
		std::string msg = vr.get("message")->stringValue();
		std::string msg2 = StringUtil::format("download %d,%s fail - %s", fstype, fileName.c_str(), msg.c_str());
		CCLOG("Upgrade: %s", msg2.c_str());
		finishWork(UPGRADE_STATUS_FAIL, msg2.c_str());
		return;
	}	
	
	if(vr.get("status")->intValue()!=200) {
		std::string msg2 = StringUtil::format("download %d,%s invalid response - %d", fstype, fileName.c_str(), vr.get("status")->intValue());
		CCLOG("Upgrade: %s", msg2.c_str());
		finishWork(UPGRADE_STATUS_FAIL, msg2.c_str());
		return;
	}

	std::string content = vr.get("content")->stringValue();
	if(type==0) {
		// query
		addWorks(content);
		CCLOG("Upgrade: query version %s done", m_config.version.c_str());
		finishWork(UPGRADE_STATUS_PROCESS, NULL);
	} else {
		if(!CCFileSystem::sharedFileSystem()->fileWriteString(cocos2d::FileSystemType(fstype), fileName.c_str(), content)) {
			std::string msg2 = StringUtil::format("download %d,%s save fail", fstype, fileName.c_str());
			CCLOG("Upgrade: %s", msg2.c_str());
			finishWork(UPGRADE_STATUS_FAIL, msg2.c_str());	
			return;
		}

		std::string msg = StringUtil::format("download %d,%s done", fstype, fileName.c_str());
		CCLOG("Upgrade: %s", msg.c_str());
		finishWork(UPGRADE_STATUS_PROCESS, msg.c_str());
	}	
	doProcess();
}

void CCEUpgradeManager::queryInfo(UpgradeInfo* info)
{
	info->end = m_info.end;
	info->fail = m_info.fail;
	info->finishWork = m_info.finishWork;
	info->message = m_info.message;
	info->totalWork = m_info.totalWork;
}

bool CCEUpgradeManager::cancel()
{
	return false;
}

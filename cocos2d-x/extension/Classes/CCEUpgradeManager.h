#ifndef  __CCE_UPGRADE_MANAGER_H__
#define  __CCE_UPGRADE_MANAGER_H__

#include "cocos2d.h"

USING_NS_CC;

#define UPGRADE_STEP_QUERY		0
#define UPGRADE_STEP_PROCESS	1
typedef struct _UpgradeConfig {
	std::string url;
	std::string host;
	std::string version;
	int step;
} UpgradeConfig;

typedef struct _UpgradeInfo {
	bool end;
	bool fail;
	int finishWork;
	int totalWork;
	std::string message;
} UpgradeInfo;

#define UPGRADE_STATUS_END		0
#define UPGRADE_STATUS_PROCESS	1
#define UPGRADE_STATUS_FAIL		-1	

class CCEUpgradeManager
{
public:
	CCEUpgradeManager();
	~CCEUpgradeManager();

public:
	static CCEUpgradeManager* sharedUpgradeManager(void);

	void getConfig(UpgradeConfig* cfg);
	bool setConfig(UpgradeConfig* cfg);

	bool isProcessing();
	bool start();
	void queryInfo(UpgradeInfo* info);
	bool cancel();

	void handleDownload(CCValueArray& params, int type, int fstype, std::string fileName);

protected:
	void addWorks(std::string s);
	void doProcess();
	bool _doProcess();
	void download(std::string url, std::string host, CCValueMap* params, int type, int fstype, std::string fileName);
	void finishWork(int status, const char* msg);

protected:	
	bool m_processing;
	UpgradeConfig m_config;
	UpgradeInfo m_info;	
	std::vector<std::string> m_workList;

	int m_httpclientRequestId;
};

#endif // __CCE_SOCKET_H__


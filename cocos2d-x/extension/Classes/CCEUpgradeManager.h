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

#define UPGRADE_STATUS_END		0
#define UPGRADE_STATUS_PROCESS	1
#define UPGRADE_STATUS_FAIL		-1	
typedef bool (CALLBACK *CCEUpgradeProcesser)(void* data, int status, int remainStep, float currentStepPer);

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
	bool start(CCEUpgradeProcesser processer,void *data);
	bool cancel();

	void handleDownload(CCValueArray& params, int type, int fstype, std::string fileName);

protected:
	void doProcess();
	bool _doProcess();
	void download(std::string url, std::string host, CCValueMap* params, int type, int fstype, std::string fileName);
	void callback(int status, float per);

protected:	
	CCEUpgradeProcesser m_processer;
	void* m_processerData;

	bool m_processing;
	UpgradeConfig m_config;
	std::vector<std::string> m_stepList;

	int m_httpclientRequestId;
};

#endif // __CCE_SOCKET_H__


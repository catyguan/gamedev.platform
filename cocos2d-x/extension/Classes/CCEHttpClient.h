#ifndef  __CCE_HTTPCLIENT_H__
#define  __CCE_HTTPCLIENT_H__

#include "cocos2d.h"

USING_NS_CC;

struct curl_slist;
typedef struct _HttpClientReq {
	int id;
	std::string url;
	size_t bufferSize;
	CCValue callback;

	void* ch;
	bool added;
	std::vector<std::string> headers;
	char* data;
	size_t datasize;
	size_t size;
	curl_slist* headerList;
} HttpClientReq;
class CCEHttpClient
{
public:
	CCEHttpClient();
	~CCEHttpClient();

public:
	static CCEHttpClient* sharedHttpClient(void);
	static void purgeSharedHttpClient(void);

	static void CALLBACK appRunnable(void* data, long mstick);

	int process(CCValue& reqData, CCValue callback);
	int queryRunningCount();
	std::string escape(std::string v);
	bool cancel(int reqId);

protected:
	HttpClientReq* create(int reqId, CCValue& repData);
	void delreq(HttpClientReq* req);

	void cleanup();	

protected:
	int m_idSeq;	
	bool m_globalInit;

	std::vector<HttpClientReq*> m_reqs;
	void* m_handle;
};

#endif // __CCE_SOCKET_H__


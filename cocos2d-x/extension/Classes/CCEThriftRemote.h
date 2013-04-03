#ifndef __CCE_THRIFT_REMOTE_H__
#define __CCE_THRIFT_REMOTE_H__

#include "cocos2d.h"
#include "../thrift/protocol/TProtocol.h"
#include "CCEAsynSocket.h"

USING_NS_CC;

class CCEThriftResponseHandler
{
public:
	virtual ~CCEThriftResponseHandler(){};

	virtual bool onTimeout() = 0;
	virtual bool handleResponse(CCValueArray& msg) = 0;
	virtual bool handleError(CCValue& err) = 0;
};

class CCEThriftRemoteService
{
public:
	virtual ~CCEThriftRemoteService(){};

	virtual bool handleRequest(int seqid, std::string module, std::string name, int type, CCValueBuilder* msg) = 0;
};

class CCEThriftRemote
{
public:
	void install();
	static CCEThriftRemote* sharedRemote();
	static void purgeSharedRemote();
	
	CCEThriftRemote();
	virtual ~CCEThriftRemote();

	void addGate(const char* hostname, int port);
	void clearGate();
	void setRetryConnectTime(int time); // time<-1 disable auto retry connect
	void setFrameMaxLength(int v){m_frameMaxLength = v;};
	int getFrameMaxLength(){return m_frameMaxLength;};
	CCEThriftRemoteService* setService(CCEThriftRemoteService* s) {
		CCEThriftRemoteService* r = m_service;
		m_service = s;
		return r;
	};
	CCEThriftRemoteService* getService(){return m_service;};
	
	bool isConnected();
	virtual bool connect() = 0;

	virtual int call(std::string name,CCValue& msg, CCEThriftResponseHandler* handler, int timeout) = 0;
	virtual bool onewayCall(std::string name, CCValue& msg);
	virtual void discardCall(int callId) = 0;

	static void CALLBACK appRunnable(void* data, long mstick);

protected:
	virtual void start();
	virtual void cleanup();

	std::pair<std::string, int>* currentGate();
	void nextGate();
	
protected:
	std::vector<std::pair<std::string, int>> m_gateList;
	unsigned int m_gateIndex;

	int m_frameMaxLength;
	int m_retryConnectTime;
	long m_lastConnectTime;
	CCEThriftRemoteService* m_service;

	CCEAsynSocket m_socket;	
};

#endif // __CCE_THRIFT_REMOTE_H__

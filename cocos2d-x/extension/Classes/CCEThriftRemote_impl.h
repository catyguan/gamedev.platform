#ifndef __CCE_THRIFT_REMOTE_IMPL_H__
#define __CCE_THRIFT_REMOTE_IMPL_H__

#include "cocos2d.h"
#include "CCEThriftRemote.h"
#include "../thrift/protocol/TProtocol.h"
#include "../thrift/TApplicationException.h"

USING_NS_CC;
using namespace ::apache::thrift::protocol;

class CCEThriftResponseHandlerSimple : public CCEThriftResponseHandler
{
public:
	CCEThriftResponseHandlerSimple(CCValue* result, CCValue* error, CCValue* timeout);
	virtual ~CCEThriftResponseHandlerSimple();

	virtual bool onTimeout();
	virtual bool handleResponse(CCValueArray& msg);
	virtual bool handleError(CCValue& err);

protected:
	CCValue resultCall;
	CCValue errorCall;
	CCValue timeoutCall;
};

class CCEThriftRemoteServiceSimple : public CCEThriftRemoteService
{
public:
	CCEThriftRemoteServiceSimple(CCValue* call);
	virtual ~CCEThriftRemoteServiceSimple();

	virtual bool handleRequest(int seqid, std::string module, std::string name, int type, CCValueBuilder* msg);

protected:
	CCValue call;
};

class CCEThriftRemoteCall
{
public:
	~CCEThriftRemoteCall();

public:
	int seqId;
	std::string name;
	TMessageType type;
	TScheme* argScheme;
	TScheme* retScheme;
	CCValue value;
	CCEThriftResponseHandler* handler;
	bool waiting;
};

typedef struct _RemoteResponseHandler {
	TScheme* scheme;
	CCEThriftResponseHandler* handler;
} ThriftRemoteResponseHandler;

class CCEThriftRemoteSimple : public CCEThriftRemote, CCEAsynSocketHandler
{
public:
	CCEThriftRemoteSimple();
	virtual ~CCEThriftRemoteSimple();

	virtual bool connect() {return checkConnect(false);};
	virtual int call(std::string name, CCValue& msg, CCEThriftResponseHandler* handler, int timeout);	
	virtual void discardCall(int callId);
	
public:
	virtual bool handleUpstream(CCEAsynSocketEvent* e);

	void thriftSend(int seqid, std::string name, 
		TMessageType type, 
		TScheme* scheme, 
		CCValue* data);
	void thriftSend(int seqid, std::string name, ::apache::thrift::TApplicationException* ex);
	

	CCEAsynSocket* socket(){return &m_socket;};
	
protected:	
	virtual void start();
	virtual void cleanup();

	virtual void onRemoteConnected();
	virtual void onRemoteClose();
	
	void split(std::string& target, std::string& module, std::string& name);
	
	void resetBuffer();
	bool checkConnect(bool f);
	void send(CCEThriftRemoteCall* call);
	int handleData(const char* data,int len);

protected:	
	bool m_connecting;
	std::list<CCEThriftRemoteCall*> m_await;
	std::map<int, ThriftRemoteResponseHandler> m_callbacks;
	
	int m_headerPos;
	uint32_t m_header;

	int m_dataPos;
	int m_dataSize;
	int m_dataLen;
	char* m_data;

	int m_seqId;
};

#endif // __CCE_THRIFT_REMOTE_H__

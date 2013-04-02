#ifndef __CCE_ASYN_SOCKET_H__
#define __CCE_ASYN_SOCKET_H__

#include "CCEThread.h"
#include "CCESocket.h"

class CCEAsynSocket;
class CCEAsynSocketEvent
{
public:
	CCEAsynSocketEvent() : m_socket(NULL) {};
	virtual ~CCEAsynSocketEvent(){};
	
	CCEAsynSocket* socket(){return m_socket;}
	void bind(CCEAsynSocket* s){m_socket = s;}

	virtual std::string debugString() = 0;

protected:
	std::string format(const char* format, ...);
	
protected:
	CCEAsynSocket* m_socket;
};

class CCEAsynSocketEventOpen : public CCEAsynSocketEvent
{
public:
	CCEAsynSocketEventOpen(bool open, bool conn) {
		m_open = open;
		m_connect = conn;
	};
	virtual ~CCEAsynSocketEventOpen(){};
	
	bool isOpen(){return m_open;};
	bool isConnect(){return m_connect;};

	virtual std::string debugString() {
		return format("EventOpen - %s, %s", m_open?"open":"close", m_connect?"connect":"");
	}

protected:
	bool m_open;
	bool m_connect;
};

class CCEAsynSocketEventTimeout : public CCEAsynSocketEvent
{
public:
	CCEAsynSocketEventTimeout(std::string n, void* d) {
		name = n;
		data = d;
	};
	virtual ~CCEAsynSocketEventTimeout(){};
	
	std::string getName(){return name;};
	bool isName(const char* n){return name.compare(n)==0;};

	void* getData(){return data;};

	virtual std::string debugString() {
		return format("EventTimeout - %s, %p", name.c_str(), data);
	}

protected:
	std::string name;
	void* data;
};

class CCEAsynSocketEventRead : public CCEAsynSocketEvent
{
public:
	CCEAsynSocketEventRead(const char* b, int l) {
		buf = new char[l];
		memcpy(buf,b,l);
		len = l;
	}
	virtual ~CCEAsynSocketEventRead(){
		if(buf!=NULL)delete buf;
	};
	
	const char* getData(){return buf;};
	int getSize(){return len;};

	virtual std::string debugString() {
		return format("EventRead - %d", len);
	}
	
protected:
	char* buf;
	int len;
};

class CCEAsynSocketEventWrite : public CCEAsynSocketEvent
{
public:
	CCEAsynSocketEventWrite(int id, bool done) {
		this->id = id;
		this->done = done;
	}
	virtual ~CCEAsynSocketEventWrite(){};
	
	int getId(){return id;};
	bool isDone(){return done;};

	virtual std::string debugString() {
		return format("EventWrite - %d, %s", id, done?"done":"fail");
	}
	
protected:
	int id;
	bool done;
};

class CCEAsynSocketHandler
{
public:
	virtual bool handleUpstream(CCEAsynSocketEvent* e) = 0;
};

typedef struct _CCEAsynSocketTimeoutItem {
	std::string name;
	void* data;
	long time;
} CCEAsynSocketTimeoutItem;

class CCEAsynSocket : public CCEThread
{
public:
	CCEAsynSocket();
	virtual ~CCEAsynSocket();

	// main API
	bool isOpen();
	bool open(const char* host,int port);	
	bool write(int id, const char* buf, int len);
	void close();

	void addTimeout(std::string name, void* data, long tm);
	void removeTimeout(const char* name);
		
	CCEAsynSocketHandler* getHandler();
	CCEAsynSocketHandler* setHandler(CCEAsynSocketHandler* handler);
	
	static void CALLBACK appRunnable(void* data, long mstick);
	
protected:	
	// thread override
	virtual void run();
	virtual void cleanup();
	virtual void wakeup();
	virtual bool processCommand(CCEThreadCommand* cmd);
	
	// main helper
	void mainProcessEvents();

	// thread helper
	void threadConnect(const char* host, int port);
	void threadCheckServer();
	bool threadServerWrite(int id, const char* buf,int len);
	bool threadServerRead();
	void threadCloseSocket();

	void threadSendEvent(CCEAsynSocketEvent* ev);

protected:
	CCESocket m_socket;
	CCEAsynSocketHandler* m_handler;
	
	pthread_mutex_t m_eventLock;
	std::list<CCEAsynSocketEvent*> m_events;

	std::list<CCEAsynSocketTimeoutItem> m_timeoutItems;

	
	
	pthread_mutex_t m_thisLock;
	bool m_socketOpen;

	char* m_readBuffer;
	int m_readBufferSize;
};

#endif // __CCE_ASYN_SOCKET_H__

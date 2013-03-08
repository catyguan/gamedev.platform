#ifndef  __CCE_SOCKET_H__
#define  __CCE_SOCKET_H__

#include "cocos2d.h"

class CCESocketMessage
{

};

class CCESocketHandler
{
public:
	CCESocketHandler(){};
	virtual ~CCESocketHandler(){};

	virtual void run() = 0;
};

class CCESocket
{
public:
	CCESocket();
	virtual ~CCESocket();

	void createFakeFD();
	bool open(const char* host,int port);
	bool isOpen(){return m_sockfd!=0;};
	bool write(const char* buf, int len);
	int read(char* buf,int len, int time);
	void close();
	void clear();	

	void wakeup();
protected:
	int m_sockfd;
	int m_fakefd_w;
	int m_fakefd_r;
};

#endif // __CCE_SOCKET_H__


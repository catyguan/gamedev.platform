#ifndef  __CCE_THREAD_H__
#define  __CCE_THREAD_H__

#include "cocos2d.h"
#include <list>
#include <pthread.h>

class CCEThread;
class CCEThreadCommand
{
public:
	CCEThreadCommand(){};
	virtual ~CCEThreadCommand(){};
};

class CCEThreadCommand4Runnable
{
public:
	CCEThreadCommand4Runnable(){};
	virtual ~CCEThreadCommand4Runnable(){};

	virtual void run() = 0;
};

class CCEThreadEvent
{
public:
	CCEThreadEvent();
	virtual ~CCEThreadEvent();

	bool create();
	bool isValid();
	bool waitSign();
	bool waitSign(int time);
	void sign();
	void close();

	void lock();
	void unlock();

protected:
	pthread_mutex_t m_lock;
	pthread_cond_t m_cond;
};

class CCEThread
{
public:    
	CCEThread();
	virtual ~CCEThread();
	
	bool start();
	void stop();
	bool sendCommand(CCEThreadCommand* cmd);

	static void* thread_main(void *arg);

protected:
	virtual bool init();	
	virtual void run();
	virtual void wakeup();
	virtual void cleanup();
	bool isLive(){return m_valid;};

	bool processCommands(int wait);
	CCEThreadCommand* peekCommand();
	virtual bool processCommand(CCEThreadCommand* cmd);
	
protected:
	bool m_valid;
	pthread_t m_thread;
	CCEThreadEvent m_event;
	std::list<CCEThreadCommand*> m_commands;
};

#endif // __CCE_THREAD_H__


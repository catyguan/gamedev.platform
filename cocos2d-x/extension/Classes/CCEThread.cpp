#include "CCETHread.h"

USING_NS_CC;

// CCEThreadEvent
CCEThreadEvent::CCEThreadEvent()
	: m_lock(PTHREAD_MUTEX_INITIALIZER)
	, m_cond(PTHREAD_COND_INITIALIZER)
{
}

CCEThreadEvent::~CCEThreadEvent()
{
	close();
}

bool CCEThreadEvent::create()
{
	int err;
	err = pthread_mutex_init(&m_lock,NULL);
	if (err != 0) {
		CCLOG("can't create mutex: %s", strerror(err));
		return false;
	}
	err = pthread_cond_init(&m_cond,NULL);
	if (err != 0) {
		CCLOG("can't create cond: %s", strerror(err));
		return false;
	}
	return true;
}

bool CCEThreadEvent::isValid()
{
	return m_cond!=PTHREAD_COND_INITIALIZER;
}

bool CCEThreadEvent::waitSign()
{
	CC_ASSERT(isValid());
	if(!isValid())return false;

	pthread_mutex_lock(&m_lock);   
	int r = pthread_cond_wait(&m_cond,&m_lock);   
	pthread_mutex_unlock(&m_lock);  
	return r==0;
}

bool CCEThreadEvent::waitSign(int time)
{
	if(!isValid())return false;

	timeval tv;
	timespec tim;
	gettimeofday(&tv, NULL);
    tim.tv_sec = time / 1000 + tv.tv_sec + (time % 1000 + tv.tv_usec / 1000) / 1000;
	tim.tv_nsec    = (time % 1000 * 1000 + tv.tv_usec) % 1000000 * 1000;  
	pthread_mutex_lock(&m_lock);   
	int r = pthread_cond_timedwait(&m_cond,&m_lock,&tim);
	pthread_mutex_unlock(&m_lock);  
	return r==0;
}

void CCEThreadEvent::sign()
{
	if(isValid()) {
		pthread_cond_signal(&m_cond);
	}
}

void CCEThreadEvent::lock()
{
	if(!isValid())return;
	pthread_mutex_lock(&m_lock);   
}

void CCEThreadEvent::unlock()
{
	if(!isValid())return;
	pthread_mutex_unlock(&m_lock);   
}

void CCEThreadEvent::close()
{
	if(m_cond!=PTHREAD_COND_INITIALIZER) {
		pthread_cond_destroy(&m_cond);
		m_cond = PTHREAD_COND_INITIALIZER;
	}
	if(m_lock!=PTHREAD_MUTEX_INITIALIZER) {
		pthread_mutex_destroy(&m_lock);
		m_lock = PTHREAD_MUTEX_INITIALIZER;
	}
}

// CCEThread
CCEThread::CCEThread()
	: m_valid(false)
{
	memset(&m_thread,0,sizeof(m_thread));
}

CCEThread::~CCEThread()
{
	m_event.close();
}

typedef struct {
	CCEThreadEvent done;
	CCEThread* thread;
} thread_start ;

void* CCEThread::thread_main(void *arg)
{
	thread_start* ts = (thread_start*) arg;
	CCEThread* th = ts->thread;	
	th->init();
	ts->done.sign();
	th->run();
	th->cleanup();
	return NULL;
}

bool CCEThread::start()
{
	CC_ASSERT(m_thread.p==NULL);	
	thread_start ts;
	ts.thread = this;
	if(!ts.done.create()) {
		return false;
	}
	m_valid = true;
	int err = pthread_create(&m_thread, NULL, thread_main, &ts);
	if (err != 0) {
		m_valid = false;
		CCLOG("can't create thread: %s", strerror(err));
		return false;
	}
	ts.done.waitSign();
	return true;
}

class StopCommand : public CCEThreadCommand
{
public:
	virtual ~StopCommand(){};
};

void CCEThread::stop()
{
	if(!isLive())return;

	StopCommand* cmd = new StopCommand();
	sendCommand(cmd);
	void* r;
	pthread_join(m_thread, &r);
}

bool CCEThread::sendCommand(CCEThreadCommand* cmd)
{
	if(!isLive()) {
		return false;
	}

	m_event.lock();
	m_commands.push_back(cmd);
	wakeup();
	m_event.sign();
	m_event.unlock();	
	wakeup();
	return true;
}

bool CCEThread::init()
{
	if(!m_event.create()) {
		return false;
	}
	return true;
}

void CCEThread::run()
{
	while(isLive()) {
		processCommands(-1);
	}
}

void CCEThread::wakeup()
{

}

void CCEThread::cleanup()
{
	m_event.lock();
	std::list<CCEThreadCommand*>::const_iterator it;
	while(m_commands.size()>0) {
		it = m_commands.begin();
		delete *it;
		m_commands.erase(it);
	}
	m_event.unlock();
}

CCEThreadCommand* CCEThread::peekCommand()
{
	CCEThreadCommand* cmd = NULL;
	std::list<CCEThreadCommand*>::const_iterator it;

	m_event.lock();	
	if(m_commands.size()>0) {
		it = m_commands.begin();
		cmd = *it;
		m_commands.erase(it);
	}
	m_event.unlock();
	return cmd;
}

bool CCEThread::processCommands(int wait)
{
	CCEThreadCommand* cmd;
	while((cmd = peekCommand())!=NULL) {
		bool r = processCommand(cmd);
		delete cmd;
		if(!r) {			
			return false;
		}
	}

	if(wait<0) {
		m_event.waitSign();
	} else if(wait>0) {
		m_event.waitSign(wait);
	}
	return m_valid;
}

bool CCEThread::processCommand(CCEThreadCommand* th)
{
	if(true) {
		StopCommand* cmd = dynamic_cast<StopCommand*>(th);
		if(cmd!=NULL) {
			CCLOG("StopCommand!!");
			m_valid = false;
			return false;
		}
	}
	if(true) {
		CCEThreadCommand4Runnable* cmd = dynamic_cast<CCEThreadCommand4Runnable*>(th);
		if(cmd!=NULL) {
			cmd->run();
			return true;
		}
	}
	return true;
}



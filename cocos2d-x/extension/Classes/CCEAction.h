#ifndef  __CCE_ACTION_UTIL_CLASS_H__
#define  __CCE_ACTION_UTIL_CLASS_H__

#include "cocos2d.h"

USING_NS_CC;

class CCECall : public CCActionInstant
{
public:
    //super methods
    virtual void update(float time);
    virtual CCFiniteTimeAction * reverse(void);
    virtual CCObject* copyWithZone(cocos2d::CCZone *pZone);

	virtual void cleanup();

public:
    /** Allocates and initializes the action */
	static CCECall* create(CCValue v);
	static CCECall* create(CCObject* obj,const char* method, CCValueArray& ps);
	static CCECall* create(CCObject* obj,const char* method);
	static CCECall* create(CCObject* obj,const char* method, CCValue p1);
	static CCECall* create(CCObject* obj,const char* method, CCValue p1, CCValue p2);
	static CCECall* create(CCObject* obj,const char* method, CCValue p1, CCValue p2, CCValue p3);

protected:
	void initCall(CCValue v);

	CCECall();
	virtual ~CCECall();

protected:
	CCValue m_call;
};

// CCECallInterval
class CCECallInterval : public CCActionInterval
{
public:
    //super methods
	virtual CCObject* copyWithZone(cocos2d::CCZone *pZone);
    virtual void update(float time);
    virtual CCActionInterval* reverse(void);    

	virtual void cleanup();

public:
    /** Allocates and initializes the action */
	static CCECallInterval* create(float d, CCValue v);
	static CCECallInterval* create(float d, CCObject* obj,const char* method, CCValueArray& ps);
	static CCECallInterval* create(float d, CCObject* obj,const char* method);
	static CCECallInterval* create(float d, CCObject* obj,const char* method, CCValue p1);
	static CCECallInterval* create(float d, CCObject* obj,const char* method, CCValue p1, CCValue p2);
	static CCECallInterval* create(float d, CCObject* obj,const char* method, CCValue p1, CCValue p2, CCValue p3);

protected:
	void initCall(float d, CCValue v);

	CCECallInterval();
	virtual ~CCECallInterval();

protected:
	float m_duration;
	CCValue m_call;
};

class CCEActionUtil
{
public:
		
};

#endif
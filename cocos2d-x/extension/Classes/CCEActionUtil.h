#ifndef  __CCE_ACTION_UTIL_CLASS_H__
#define  __CCE_ACTION_UTIL_CLASS_H__

#include "cocos2d.h"

USING_NS_CC;

class CCELuaResponseAction : public cocos2d::CCActionInstant
{
public:
    //super methods
    virtual void update(float time);
    virtual cocos2d::CCFiniteTimeAction * reverse(void);
    virtual cocos2d::CCObject* copyWithZone(cocos2d::CCZone *pZone);

public:
    /** Allocates and initializes the action */
	static CCELuaResponseAction* create(int callId, CCValueArray& ret);

protected:
	void initLuaResponse(int callId, CCValueArray& ret);

	CCELuaResponseAction();
	virtual ~CCELuaResponseAction();

protected:
	int callId;
	CCValueArray m_Return;
};

class CCELuaCallAction : public cocos2d::CCActionInstant
{
public:
    //super methods
    virtual void update(float time);
    virtual cocos2d::CCFiniteTimeAction * reverse(void);
    virtual cocos2d::CCObject* copyWithZone(cocos2d::CCZone *pZone);

public:
    /** Allocates and initializes the action */
	static CCELuaCallAction* create(const char* fun, CCValueArray& ps);

protected:
	void initLuaCall(const char* fun, CCValueArray ps);

	CCELuaCallAction();
	virtual ~CCELuaCallAction();

protected:
	std::string m_csFun;
	CCValueArray m_Params;
};

class CCEActionUtil
{
public:
		
};

#endif
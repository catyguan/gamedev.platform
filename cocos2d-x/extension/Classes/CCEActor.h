#ifndef  __CCE_ACTOR_H__
#define  __CCE_ACTOR_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEActor : public cocos2d::CCObject
{
public:
	CCEActor();
	virtual ~CCEActor();

public:
	virtual bool init();
	virtual void cleanup();

protected:
	
};

class CCEActorCommand : public cocos2d::CCActionInstant
{
public:
    //super methods
    virtual void update(float time);
    virtual cocos2d::CCFiniteTimeAction * reverse(void);
    virtual cocos2d::CCObject* copyWithZone(cocos2d::CCZone *pZone);

public:
    /** Allocates and initializes the action */
	static CCEActorCommand* create(CCEActor* actor, std::string name, CCValueArray& ctx);

protected:
	void initActorCommand(CCEActor* p,std::string n,CCValueArray ps);

	CCEActorCommand();
	virtual ~CCEActorCommand();

protected:
	CCEActor* m_pActor;
	std::string m_csName;
	CCValueArray m_Params;
};

class CCEActorSprite : public CCEActor
{
public:
	CCEActorSprite();
	virtual ~CCEActorSprite();

public:
	cocos2d::CCSprite* getSprite() {return m_pSprite;};

	void initSprit(cocos2d::CCSprite* sprite);

	virtual void cleanup();

public:
	// delegate
	cocos2d::CCAction* runAction(cocos2d::CCAction* action){return m_pSprite->runAction(action);}
	void stopAllActions(void){m_pSprite->stopAllActions();}
    void stopAction(cocos2d::CCAction* action){m_pSprite->stopAction(action);}
    void stopActionByTag(int tag){m_pSprite->stopActionByTag(tag);}
	cocos2d::CCAction* getActionByTag(int tag){return m_pSprite->getActionByTag(tag);}

protected:
	cocos2d::CCSprite* m_pSprite;
};

#endif
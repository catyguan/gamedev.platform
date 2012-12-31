#include "CCEActor.h"

USING_NS_CC;

// CCEActor
CCEActor::CCEActor()
{
    
}

CCEActor::~CCEActor()
{
	
}

bool CCEActor::init()
{
	return true;
}

void CCEActor::cleanup()
{
	
}

// CCEActorCommand
CCEActorCommand::CCEActorCommand()
{
	m_pActor = NULL;
}

void CCEActorCommand::initActorCommand(CCEActor* p,std::string n,CCValueArray ps)
{
	m_pActor = p;
	p->retain();
	m_csName = n;
	m_Params = ps;
}

CCEActorCommand::~CCEActorCommand()
{
	CC_SAFE_RELEASE_NULL(m_pActor);
}

void CCEActorCommand::update(float time) {
    CC_UNUSED_PARAM(time);
    CCValueArray ps(m_Params);
	try {
		// m_pActor->processCommand(CCELuaHost::sharedLuaHost(),-1,m_csName,ps);
	} catch(std::string err) {
		CCLOG("CCEActorCommand: ERROR - [%s] fail, %s",m_csName.c_str(),err.c_str());
	}
}

CCFiniteTimeAction* CCEActorCommand::reverse() {
    return NULL;
}

CCObject * CCEActorCommand::copyWithZone(CCZone *pZone) {
    CCZone *pNewZone = NULL;
    CCEActorCommand *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCEActorCommand*) (pZone->m_pCopyObject);
    } else {
		pRet = new CCEActorCommand();
        pZone = pNewZone = new CCZone(pRet);
    }

    CCActionInstant::copyWithZone(pZone);    
	pRet->initActorCommand(m_pActor, m_csName, m_Params);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

CCEActorCommand* CCEActorCommand::create(CCEActor* actor, std::string name, CCValueArray& ctx)
{
	CCEActorCommand* r = new CCEActorCommand();
	r->initActorCommand(actor,name,ctx);
	r->autorelease();
	return r;
}

// CCEACtorSprite
CCEActorSprite::CCEActorSprite()
{
	m_pSprite = NULL;
}

CCEActorSprite::~CCEActorSprite()
{
	CC_SAFE_RELEASE_NULL(m_pSprite);
}

void CCEActorSprite::initSprit(cocos2d::CCSprite* sprite)
{
	CC_ASSERT(m_pSprite==NULL);
	m_pSprite = sprite;
	m_pSprite->retain();
}

void CCEActorSprite::cleanup()
{
	CC_SAFE_RELEASE_NULL(m_pSprite);
}

#include "CCEActionUtil.h"
#include "CCELuaApplication.h"

USING_NS_CC;

// CCELuaResponseAction
CCELuaResponseAction::CCELuaResponseAction()
{
	callId = 0;
}

void CCELuaResponseAction::initLuaResponse(int cid,CCValueArray& ps)
{
	callId = cid;
	m_Return = ps;
}

CCELuaResponseAction::~CCELuaResponseAction()
{
	
}

void CCELuaResponseAction::update(float time) {
    CC_UNUSED_PARAM(time);
	if(callId!=0) {	
		CCELuaApplication::sharedLuaHost()->callResponse(callId,NULL,m_Return);	
		callId = 0;
	}
}

CCFiniteTimeAction* CCELuaResponseAction::reverse() {
    return NULL;
}

CCObject * CCELuaResponseAction::copyWithZone(CCZone *pZone) {
    CCZone *pNewZone = NULL;
    CCELuaResponseAction *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCELuaResponseAction*) (pZone->m_pCopyObject);
    } else {
		pRet = new CCELuaResponseAction();
        pZone = pNewZone = new CCZone(pRet);
    }

    CCActionInstant::copyWithZone(pZone);    
	pRet->initLuaResponse(callId, m_Return);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

CCELuaResponseAction* CCELuaResponseAction::create(int cid, CCValueArray& rt)
{
	CCELuaResponseAction* r = new CCELuaResponseAction();
	r->initLuaResponse(cid, rt);
	r->autorelease();
	return r;
}

// CCELuaCallAction
CCELuaCallAction::CCELuaCallAction()
{
	
}

void CCELuaCallAction::initLuaCall(const char* fun,CCValueArray ps)
{
	m_csFun = fun;
	m_Params = ps;
}

CCELuaCallAction::~CCELuaCallAction()
{
	
}

void CCELuaCallAction::update(float time) {
    CC_UNUSED_PARAM(time);
	CCELuaApplication::sharedLuaHost()->pcall(m_csFun.c_str(),m_Params);
}

CCFiniteTimeAction* CCELuaCallAction::reverse() {
    return NULL;
}

CCObject * CCELuaCallAction::copyWithZone(CCZone *pZone) {
    CCZone *pNewZone = NULL;
    CCELuaCallAction *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCELuaCallAction*) (pZone->m_pCopyObject);
    } else {
		pRet = new CCELuaCallAction();
        pZone = pNewZone = new CCZone(pRet);
    }

    CCActionInstant::copyWithZone(pZone);    
	pRet->initLuaCall(m_csFun.c_str(), m_Params);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

CCELuaCallAction* CCELuaCallAction::create(const char* fun, CCValueArray& rt)
{
	CCELuaCallAction* r = new CCELuaCallAction();
	r->initLuaCall(fun, rt);
	r->autorelease();
	return r;
}

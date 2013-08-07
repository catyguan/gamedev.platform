#include "CCEAction.h"
#include "cocoa/CCValueSupport.h"

USING_NS_CC;

// CCECall
CCECall::CCECall()
{
	
}

CCECall::~CCECall()
{
	m_call.cleanup();
}

void CCECall::cleanup()
{
	m_call.cleanup();
}

void CCECall::initCall(CCValue v)
{
	CC_ASSERT(v.canCall());
	m_call = v;
	m_call.retain();
}

void CCECall::update(float time) {
    CC_UNUSED_PARAM(time);
    CCValueArray ps;
	m_call.call(ps,false);
}

CCFiniteTimeAction* CCECall::reverse() {
    return NULL;
}

CCObject * CCECall::copyWithZone(CCZone *pZone) {
    CCZone *pNewZone = NULL;
    CCECall *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCECall*) (pZone->m_pCopyObject);
    } else {
		pRet = new CCECall();
        pZone = pNewZone = new CCZone(pRet);
    }

    CCActionInstant::copyWithZone(pZone);    
	pRet->initCall(m_call);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

CCECall* CCECall::create(CCValue v)
{
	if(!v.canCall())return NULL;

	CCECall* r = new CCECall();
	r->initCall(v);
	r->autorelease();
	return r;
}

CCECall* CCECall::create(CCObject* obj,const char* method, CCValueArray& ps)
{
	if(obj==NULL)return NULL;
	return create(CCValue::objectValue(CCCommandObject::create(obj,method,ps)));
}

CCECall* CCECall::create(CCObject* obj,const char* method)
{
	CCValueArray ps;
	return create(obj, method, ps);	
}

CCECall* CCECall::create(CCObject* obj,const char* method,CCValue p1)
{
	CCValueArray ps;
	ps.push_back(p1);
	return create(obj, method, ps);	
}

CCECall* CCECall::create(CCObject* obj,const char* method,CCValue p1, CCValue p2)
{
	CCValueArray ps;
	ps.push_back(p1);
	ps.push_back(p2);
	return create(obj, method, ps);	
}

CCECall* CCECall::create(CCObject* obj,const char* method,CCValue p1, CCValue p2, CCValue p3)
{
	CCValueArray ps;
	ps.push_back(p1);
	ps.push_back(p2);
	ps.push_back(p3);
	return create(obj, method, ps);	
}

// CCECallInterval
CCECallInterval::CCECallInterval()
{
	
}

CCECallInterval::~CCECallInterval()
{
	m_call.cleanup();
}

void CCECallInterval::cleanup()
{
	m_call.cleanup();
}

void CCECallInterval::initCall(float d, CCValue v)
{
	CC_ASSERT(v.canCall());	
	m_duration = d;
	m_call = v;
	m_call.retain();

	CCActionInterval::initWithDuration(d);
}

void CCECallInterval::update(float t)
{
    CCValueArray ps;
	ps.push_back(CCValue::numberValue(t));
	m_call.call(ps,false);
}

CCActionInterval* CCECallInterval::reverse(void)
{
	return NULL;
}

CCObject * CCECallInterval::copyWithZone(CCZone *pZone) {
    CCZone *pNewZone = NULL;
    CCECallInterval *pRet = NULL;

    if (pZone && pZone->m_pCopyObject) {
        pRet = (CCECallInterval*) (pZone->m_pCopyObject);
    } else {
		pRet = new CCECallInterval();
        pZone = pNewZone = new CCZone(pRet);
    }

    CCActionInterval::copyWithZone(pZone);    
	pRet->initCall(m_duration, m_call);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

CCECallInterval* CCECallInterval::create(float d, CCValue v)
{
	if(!v.canCall())return NULL;

	CCECallInterval* r = new CCECallInterval();
	r->initCall(d, v);
	r->autorelease();
	return r;
}

CCECallInterval* CCECallInterval::create(float d, CCObject* obj,const char* method, CCValueArray& ps)
{
	if(obj==NULL)return NULL;
	return create(d, CCValue::objectValue(CCCommandObject::create(obj,method,ps)));
}

CCECallInterval* CCECallInterval::create(float d, CCObject* obj,const char* method)
{
	CCValueArray ps;
	return create(d, obj, method, ps);	
}

CCECallInterval* CCECallInterval::create(float d, CCObject* obj,const char* method,CCValue p1)
{
	CCValueArray ps;
	ps.push_back(p1);
	return create(d, obj, method, ps);	
}

CCECallInterval* CCECallInterval::create(float d, CCObject* obj,const char* method,CCValue p1, CCValue p2)
{
	CCValueArray ps;
	ps.push_back(p1);
	ps.push_back(p2);
	return create(d, obj, method, ps);	
}

CCECallInterval* CCECallInterval::create(float d, CCObject* obj,const char* method,CCValue p1, CCValue p2, CCValue p3)
{
	CCValueArray ps;
	ps.push_back(p1);
	ps.push_back(p2);
	ps.push_back(p3);
	return create(d, obj, method, ps);	
}

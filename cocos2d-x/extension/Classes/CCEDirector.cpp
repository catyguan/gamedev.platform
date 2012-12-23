#include "CCEDirector.h"

USING_NS_CC;

static CCEDirector* s_pSharedDirector = NULL;

void CCEDirector::install(CCELuaHost* host)
{
    CC_ASSERT(s_pSharedDirector==NULL);
	s_pSharedDirector = this;

	initHost(host);
}

CCEDirector* CCEDirector::sharedDirector(void)
{
	CC_ASSERT(s_pSharedDirector!=NULL);    
    return s_pSharedDirector;
}

void CCEDirector::purgeSharedDirector(void)
{
    if (s_pSharedDirector)
    {
        delete s_pSharedDirector;
        s_pSharedDirector = NULL;
    }
}

CCEDirector::CCEDirector()
{

}

CCEDirector::~CCEDirector()
{

}

bool CCEDirector::popScene(CCELuaHost* host, void* data, int callId, CCELuaValueArray& params)
{
	params.clear();
	CCDirector::sharedDirector()->popScene();
	return true;
}

void CCEDirector::initHost(CCELuaHost* host)
{
	host->setCall("UserDefault_getForKey", CCEDirector::popScene,NULL);
}

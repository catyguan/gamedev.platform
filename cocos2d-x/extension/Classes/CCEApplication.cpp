#include "CCEApplication.h"
#include "cocoa/CCValueSupport.h"

USING_NS_CC;

static CCEApplication* s_pSharedApp = NULL;

CCEApplication::CCEApplication()
{
	
}

CCEApplication::~CCEApplication() 
{
	
}

void CCEApplication::install()
{
    CC_ASSERT(s_pSharedApp==NULL);
	s_pSharedApp = this;
}

CCEApplication* CCEApplication::sharedApp(void)
{
	CC_ASSERT(s_pSharedApp!=NULL);    
    return s_pSharedApp;
}

void CCEApplication::purgeSharedApp(void)
{
    if (s_pSharedApp)
    {
		CC_SAFE_RELEASE_NULL(s_pSharedApp);
    }
}

CCObject* CCEApplication::createObject(const char* name, const char* type, CCValueArray& ps)
{
	return  NULL;
}

CCObject* CCEApplication::findChildById(const char* id)
{
	return CCObject::findChildById(id);
}

CC_BEGIN_CALLS(CCEApplication, CCObject)
	CC_DEFINE_CALL(CCEApplication, pushScene)
	CC_DEFINE_CALL(CCEApplication, replaceScene)
	CC_DEFINE_CALL(CCEApplication, popScene)	
CC_END_CALLS(CCEApplication, CCObject)

CCValue CCEApplication::CALLNAME(pushScene)(CCValueArray& params) {	
	std::string sceneName;
	if(params.size()<1) {
		throw new std::string("miss sceneName");
	}
	sceneName = params[0].stringValue();
	CCScene* s = NULL; // getScene(sceneName, params);
	if(s==NULL) {
		throw new std::string(sceneName+" scene invalid");
	}	
	CCDirector::sharedDirector()->pushScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEApplication::CALLNAME(replaceScene)(CCValueArray& params)
{
	std::string sceneName;
	if(params.size()<1) {
		throw new std::string("miss sceneName");
	}
	sceneName = params[0].stringValue();
	CCScene* s = NULL; // CCEDirector::sharedDirector()->getScene(sceneName, ctx);
	if(s==NULL) {
		throw new std::string(sceneName+" scene invalid");
	}	
	CCDirector::sharedDirector()->replaceScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEApplication::CALLNAME(popScene)(CCValueArray& params)
{	
	CCDirector::sharedDirector()->popScene();
	return CCValue::nullValue();
}
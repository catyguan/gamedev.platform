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
		s_pSharedApp->cleanup();
		CC_SAFE_RELEASE_NULL(s_pSharedApp);
    }
}

void CCEApplication::addObject(const char* id, CCObject* obj)
{
	CC_ASSERT(id!=NULL);
	objects.setObject(obj, id);
}

CCObject* CCEApplication::findChildById(const char* id)
{
	if(strcmp(id,"SCENE")==0) {
		return CCDirector::sharedDirector()->getRunningScene();
	}
	CCObject* obj = objects.objectForKey(id);
	if(obj!=NULL) {
		return obj;
	}
	return CCObject::findChildById(id);
}

void CCEApplication::removeObject(const char* id)
{
	objects.removeObjectForKey(id);
}

void CCEApplication::addCreateObjectFunction(CCE_CREATE_OBJECT fun)
{
	coFunctionList.push_back(fun);
}

void CCEApplication::cleanup()
{
	objects.removeAllObjects();
	coFunctionList.clear();
}

CC_BEGIN_CALLS(CCEApplication, CCObject)
	CC_DEFINE_CALL(CCEApplication, createObject)	
CC_END_CALLS(CCEApplication, CCObject)

CCValue CCEApplication::CALLNAME(createObject)(CCValueArray& params) {	
	std::string type;
	if(params.size()<1) {
		throw new std::string("miss type");
	}
	type = params[0].stringValue();
	params.erase(params.begin());
	CCObject* obj = createObject(type.c_str(), params);
	if(obj==NULL) {
		return CCValue::nullValue();
	}
	return CCValue::objectValue(obj);
}

#include "CCEDirector.h"
CCValue CCEApplication::CALLNAME(director)(CCValueArray& params) {	
	return CCValue::objectValue(CCEDirector::sharedDirector());
}
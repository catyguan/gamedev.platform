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
	CC_DEFINE_CALL(CCEApplication, pushScene)
	CC_DEFINE_CALL(CCEApplication, replaceScene)
	CC_DEFINE_CALL(CCEApplication, popScene)	
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

static CCScene* toSceneObject(CCValueArray& params,unsigned int idx)
{
	if(params.size()<=idx)return NULL;
	CCValue& v = params[0];
	if(!v.isObject()) {
		return NULL;
	}
	CCObject* o = v.objectValue();
	CCScene* r = dynamic_cast<CCScene*>(o);
	if(r!=NULL) {
		return r;
	}
	CCNode* node = dynamic_cast<CCNode*>(o);
	if(node!=NULL) {
		r = CCScene::create();
		r->addChild(node);
		return r;
	}
	return NULL;
}

CCValue CCEApplication::CALLNAME(pushScene)(CCValueArray& params) {
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 except CCScene");
	}	
	CCDirector::sharedDirector()->pushScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEApplication::CALLNAME(replaceScene)(CCValueArray& params)
{
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 except CCScene");
	}
	CCDirector::sharedDirector()->replaceScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEApplication::CALLNAME(popScene)(CCValueArray& params)
{	
	CCDirector::sharedDirector()->popScene();
	return CCValue::nullValue();
}
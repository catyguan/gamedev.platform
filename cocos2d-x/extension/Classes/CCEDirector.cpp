#include "CCEDirector.h"
#include "CCELuaApplication.h"
#include "cocoa/CCValueSupport.h"

USING_NS_CC;

static CCEDirector* s_pSharedDirector = NULL;

CCEDirector::CCEDirector(CCELuaApplication* app)
{
	m_app = app;
}


CCEDirector::~CCEDirector(void)
{
	
}

void CCEDirector::install()
{
    CC_ASSERT(s_pSharedDirector==NULL);
	s_pSharedDirector = this;
}

CCEDirector* CCEDirector::sharedDirector(void)
{
	CC_ASSERT(s_pSharedDirector!=NULL);    
    return s_pSharedDirector;
}

void CCEDirector::purgeSharedDirector(void)
{
    if (s_pSharedDirector!=NULL)
    {
		CC_SAFE_RELEASE_NULL(s_pSharedDirector);
		// delete s_pSharedDirector;
    }
}

CCValue CCEDirector::apiCall(std::string name, CCValueArray& ps)
{	
	CCValueArray rs;
	if (!m_app->pcall(name.c_str(), ps, rs))
    {		
		return CCValue::nullValue();
	}
	if(rs.size()==0) {
		return CCValue::nullValue();
	}
	return rs[0];
}

bool CCEDirector::apiStartup()
{
	CCValueArray ps;
	CCValue r = apiCall("_API_application_startup", ps);
	return r.booleanValue();
}

void CCEDirector::apiShutdown()
{
	CCValueArray ps;
	apiCall("_API_application_shutdown", ps);
}

CCValue CCEDirector::apiObjectCall(std::string name, CCValueArray& ps)
{
	ps.insert(ps.begin(), CCValue::stringValue(name));
	return apiCall("_API_application_objectcall", ps);
}

CCValue CCEDirector::apiObjectCall(std::string name)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(name));
	return apiCall("_API_application_objectcall", ps);
}

CCValue CCEDirector::apiObjectCall(std::string name, CCValue p1)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(name));
	ps.push_back(p1);
	return apiCall("_API_application_objectcall", ps);
}

CCValue CCEDirector::apiObjectCall(std::string name, CCValue p1, CCValue p2)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(name));
	ps.push_back(p1);
	return apiCall("_API_application_objectcall", ps);
}

CCValue CCEDirector::apiObjectCall(std::string name, CCValue p1, CCValue p2,CCValue p3)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(name));
	ps.push_back(p1);
	ps.push_back(p2);
	ps.push_back(p3);
	return apiCall("_API_application_objectcall", ps);
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

CC_BEGIN_CALLS(CCEDirector, CCObject)
	CC_DEFINE_CALL(CCEDirector, winSize)
	CC_DEFINE_CALL(CCEDirector, scene)
	CC_DEFINE_CALL(CCEDirector, pushScene)
	CC_DEFINE_CALL(CCEDirector, replaceScene)
	CC_DEFINE_CALL(CCEDirector, popScene)	
CC_END_CALLS(CCEDirector, CCObject)

CCValue CCEDirector::CALLNAME(winSize)(CCValueArray& params) {
	CCSize sz = CCDirector::sharedDirector()->getWinSize();
	return CCValueUtil::size(sz.width, sz.height);
}

CCValue CCEDirector::CALLNAME(scene)(CCValueArray& params) {
	CCScene* s = CCDirector::sharedDirector()->currentScene();
	return CCValue::objectValue(s);
}

CCValue CCEDirector::CALLNAME(pushScene)(CCValueArray& params) {
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 except CCScene");
	}	
	CCDirector::sharedDirector()->pushScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEDirector::CALLNAME(replaceScene)(CCValueArray& params)
{
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 except CCScene");
	}
	CCDirector::sharedDirector()->replaceScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEDirector::CALLNAME(popScene)(CCValueArray& params)
{	
	CCDirector::sharedDirector()->popScene();
	return CCValue::nullValue();
}
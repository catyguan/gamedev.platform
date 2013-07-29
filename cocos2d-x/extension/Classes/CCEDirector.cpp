#include "CCEDirector.h"
#include "CCELuaApplication.h"

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
    if (s_pSharedDirector)
    {
		// CC_SAFE_RELEASE_NULL(s_pSharedDirector);
		delete s_pSharedDirector;
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
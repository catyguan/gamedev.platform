#include "CCEConfig.h"

USING_NS_CC;

static CCEConfig s_SharedConfig;

CCEConfig::CCEConfig()
{

}


CCEConfig::~CCEConfig(void)
{
	
}

CCValue CCEConfig::get(const char* name)
{
	CCValueMapIterator it = s_SharedConfig.m_data.find(name);
	if(it!=s_SharedConfig.m_data.end()) {
		return it->second;
	}
	return CCValue::nullValue();
}

void CCEConfig::set(const char* name, CCValue v)
{
	s_SharedConfig.m_data[name] = v;
}
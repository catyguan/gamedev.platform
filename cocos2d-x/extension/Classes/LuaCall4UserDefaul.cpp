#include "LuaCall4UserDefaul.h"

#include "CCUserDefault.h"

USING_NS_CC;

void LuaCall4UserDefaul::initHost(CCELuaHost* host)
{
	host->setCall("UserDefault_getForKey", LuaCall4UserDefaul::getForKey ,NULL);
	host->setCall("UserDefault_setForKey", LuaCall4UserDefaul::setForKey ,NULL);
}

bool LuaCall4UserDefaul::getForKey(CCELuaHost* host, void* data, int callId, CCELuaValueArray& params)
{
	std::string key;
	std::string dv;
	if(params.size()>0) {
		key = params[0].stringValue();
	}
	if(params.size()>1) {
		dv = params[1].stringValue();
	}
	params.clear();
	std::string r = CCUserDefault::sharedUserDefault()->getStringForKey(key.c_str(), dv);
	params.push_back(CCELuaValue::stringValue(r));
	return true;
}

bool LuaCall4UserDefaul::setForKey(CCELuaHost* host, void* data, int callId, CCELuaValueArray& params)
{
	std::string key;
	std::string v;
	if(params.size()>0) {
		key = params[0].stringValue();
	}
	if(params.size()>1) {
		v = params[1].stringValue();
	}
	params.clear();
	CCUserDefault::sharedUserDefault()->setStringForKey(key.c_str(), v);
	return true;
}

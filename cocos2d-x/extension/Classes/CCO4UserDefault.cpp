#include "CCO4UserDefault.h"

#include "CCUserDefault.h"
#include "cocoa/CCValueSupport.h"

CC_BEGIN_CALLS(CCO4UserDefault, CCObject)
	CC_DEFINE_CALL(CCO4UserDefault, getStringForKey)
	CC_DEFINE_CALL(CCO4UserDefault, setStringForKey)
CC_END_CALLS(CCO4UserDefault, CCObject)

CCValue CCO4UserDefault::CALLNAME(getStringForKey)(CCValueArray& params)
{
	std::string key;
	std::string dv;
	if(params.size()>0) {
		key = params[0].stringValue();
	}
	if(params.size()>1) {
		dv = params[1].stringValue();
	}
	std::string r = CCUserDefault::sharedUserDefault()->getStringForKey(key.c_str(), dv);
	return CCValue::stringValue(r);
}

CCValue CCO4UserDefault::CALLNAME(setStringForKey)(CCValueArray& params)
{
	std::string key;
	std::string v;
	if(params.size()>0) {
		key = params[0].stringValue();
	}
	if(params.size()>1) {
		v = params[1].stringValue();
	}
	CCUserDefault::sharedUserDefault()->setStringForKey(key.c_str(), v);
	return CCValue::nullValue();
}
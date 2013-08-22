#include "CCO4HttpClient.h"

#include "cocoa/CCValueSupport.h"
#include "CCEHttpClient.h"
#include "CCEUtil.h"

// CCO4HttpClient
CC_BEGIN_CALLS(CCO4HttpClient, CCObject)
	CC_DEFINE_CALL(CCO4HttpClient, process)
	CC_DEFINE_CALL(CCO4HttpClient, cancel)
	CC_DEFINE_CALL(CCO4HttpClient, runningCount)
CC_END_CALLS(CCO4HttpClient, CCObject)

CCValue CCO4HttpClient::CALLNAME(process)(CCValueArray& params)
{
	if(params.size()<1) {
		throw new std::string("httpclient:process(req, callback)");		
	}
	CCValue cb = ccvp(params, 1);
	int id = CCEHttpClient::sharedHttpClient()->process(params[0], cb);
	return CCValue::intValue(id);
}

CCValue CCO4HttpClient::CALLNAME(cancel)(CCValueArray& params)
{
	int id = ccvpInt(params, 0);
	bool r = CCEHttpClient::sharedHttpClient()->cancel(id);
	return CCValue::booleanValue(r);
}

CCValue CCO4HttpClient::CALLNAME(runningCount)(CCValueArray& params)
{
	int r = CCEHttpClient::sharedHttpClient()->queryRunningCount();
	return CCValue::intValue(r);
}

CCValue CCO4HttpClient::CALLNAME(escape)(CCValueArray& params)
{
	std::string v = ccvpString(params, 0);
	std::string r = CCEHttpClient::sharedHttpClient()->escape(v);
	return CCValue::stringValue(r);
}
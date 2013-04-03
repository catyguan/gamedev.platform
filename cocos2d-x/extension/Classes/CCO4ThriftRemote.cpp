#include "CCO4ThriftRemote.h"

#include "CCEThriftRemote_impl.h"
#include "CCELuaApplication.h"
#include "cocoa/CCValueSupport.h"

CC_BEGIN_CALLS(CCO4ThriftRemote, CCObject)
	CC_DEFINE_CALL(CCO4ThriftRemote, isConnect)
	CC_DEFINE_CALL(CCO4ThriftRemote, addGate)	
	CC_DEFINE_CALL(CCO4ThriftRemote, clearGate)
	CC_DEFINE_CALL(CCO4ThriftRemote, setServiceAPI)
	CC_DEFINE_CALL(CCO4ThriftRemote, connect)
	CC_DEFINE_CALL(CCO4ThriftRemote, call)
	CC_DEFINE_CALL(CCO4ThriftRemote, onewayCall)
	CC_DEFINE_CALL(CCO4ThriftRemote, discardCall)
	CC_DEFINE_CALL(CCO4ThriftRemote, response)
CC_END_CALLS(CCO4ThriftRemote, CCObject)

CCValue CCO4ThriftRemote::CALLNAME(isConnect)(CCValueArray& params)
{
	bool r = CCEThriftRemote::sharedRemote()->isConnected();
	return CCValue::booleanValue(r);
}

CCValue CCO4ThriftRemote::CALLNAME(addGate)(CCValueArray& params)
{
	std::string hostname = ccvpString(params, 0);
	int port = ccvpInt(params,1);
	CCEThriftRemote::sharedRemote()->addGate(hostname.c_str(), port);
	return CCValue::nullValue();
}

CCValue CCO4ThriftRemote::CALLNAME(clearGate)(CCValueArray& params)
{
	CCEThriftRemote::sharedRemote()->clearGate();
	return CCValue::nullValue();
}

CCValue CCO4ThriftRemote::CALLNAME(setServiceAPI)(CCValueArray& params)
{
	std::string name = ccvpString(params, 0);
	CCValue call = CCValue::objectValue(CCELuaCallObject::create(CCELuaApplication::sharedLuaHost(), name.c_str()));
	CCEThriftRemoteService* old = CCEThriftRemote::sharedRemote()->setService(new CCEThriftRemoteServiceSimple(&call));
	if(old!=NULL) {
		delete old;
	}
	return CCValue::nullValue();
}

CCValue CCO4ThriftRemote::CALLNAME(connect)(CCValueArray& params)
{
	bool r = CCEThriftRemote::sharedRemote()->connect();
	return CCValue::booleanValue(r);
}

CCValue CCO4ThriftRemote::CALLNAME(call)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	CCValue msg = ccvp(params, 1);
	int timeout = ccvpInt(params,2);

	CCValue call = ccvp(params,3);
	CCValue err = ccvp(params,4);
	CCValue tocall= ccvp(params,5);

	CCEThriftResponseHandlerSimple* h = new CCEThriftResponseHandlerSimple(
		call.canCall()?&call:NULL,
		err.canCall()?&err:NULL,
		tocall.canCall()?&tocall:NULL
		);
	int r = CCEThriftRemote::sharedRemote()->call(name, msg, h, timeout);
	return CCValue::intValue(r);
}

CCValue CCO4ThriftRemote::CALLNAME(onewayCall)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	CCValue msg = ccvp(params, 1);
	bool r = CCEThriftRemote::sharedRemote()->onewayCall(name, msg);
	return CCValue::booleanValue(r);
}

CCValue CCO4ThriftRemote::CALLNAME(discardCall)(CCValueArray& params)
{
	int callId = ccvpInt(params, 0);
	CCEThriftRemote::sharedRemote()->discardCall(callId);
	return CCValue::nullValue();
}

CCValue CCO4ThriftRemote::CALLNAME(response)(CCValueArray& params)
{
	// TODO
	bool r = CCEThriftRemote::sharedRemote()->isConnected();
	return CCValue::booleanValue(r);
}
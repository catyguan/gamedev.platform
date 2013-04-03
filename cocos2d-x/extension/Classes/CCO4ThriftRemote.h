#ifndef  __CCO_THRIFT_REMOTE_H__
#define  __CCO_THRIFT_REMOTE_H__

#include "cocos2d.h"

USING_NS_CC;

class CCO4ThriftRemote : public CCObject
{
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(isConnect)
	CC_DECLARE_CALL(addGate)
	CC_DECLARE_CALL(clearGate)
	CC_DECLARE_CALL(setServiceAPI)
	CC_DECLARE_CALL(connect)
	CC_DECLARE_CALL(call)
	CC_DECLARE_CALL(onewayCall)
	CC_DECLARE_CALL(discardCall)
	CC_DECLARE_CALL(response)
	CC_DECLARE_CALLS_END
};

#endif
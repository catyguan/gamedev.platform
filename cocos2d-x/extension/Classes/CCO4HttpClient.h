#ifndef  __CCO_HTTPCLIENT_H__
#define  __CCO_HTTPCLIENT_H__

#include "cocos2d.h"

USING_NS_CC;

class CCO4HttpClient : public CCObject
{
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(process)
	CC_DECLARE_CALL(cancel)
	CC_DECLARE_CALL(runningCount)
	CC_DECLARE_CALL(escape)
	CC_DECLARE_CALLS_END
};

#endif
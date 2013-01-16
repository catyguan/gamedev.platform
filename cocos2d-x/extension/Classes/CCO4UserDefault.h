#ifndef  __CCO_USERDEFAULT_H__
#define  __CCO_USERDEFAULT_H__

#include "cocos2d.h"

USING_NS_CC;

class CCO4UserDefault : public CCObject
{
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(getStringForKey)
	CC_DECLARE_CALL(setStringForKey)
	CC_DECLARE_CALLS_END
};

#endif
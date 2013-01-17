#ifndef  __CCO_FILE_H__
#define  __CCO_FILE_H__

#include "cocos2d.h"

USING_NS_CC;

class CCO4File : public CCObject
{
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(getStoreFile)
	CC_DECLARE_CALLS_END
};

#endif
#ifndef  __CCE_APPUTIL_H__
#define  __CCE_APPUTIL_H__

#include "cocos2d.h"

USING_NS_CC;

class StringUtil
{
public:
	static std::string format(const char* format, ...);
};

class RectUtil
{
public:
	static CCRect rectUnion(const CCRect& src1, const CCRect& src2);
};

#endif // __CCE_APPUTIL_H_


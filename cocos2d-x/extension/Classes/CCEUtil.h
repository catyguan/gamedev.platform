#ifndef  __CCE_UTIL_H__
#define  __CCE_UTIL_H__

#include "cocos2d.h"

USING_NS_CC;

class StringUtil
{
public:
	static std::string format(const char* format, ...);
	static void split(std::string& s, std::string& delim, std::vector<std::string>* ret);
	static int replaceAll(std::string& str,  const std::string& pattern,  const std::string& newpat);
	static std::string trim(std::string& str);
};

class RectUtil
{
public:
	static CCRect rectUnion(const CCRect& src1, const CCRect& src2);
};

#endif // __CCE_APPUTIL_H_


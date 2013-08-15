#ifndef  __CCE_LAYOUT_UTIL_H__
#define  __CCE_LAYOUT_UTIL_H__

#include "cocos2d.h"

USING_NS_CC;

class LayoutUtil
{
public:
	static void layout(CCNode* ndoe, bool deep);
	static void doLayout(CCNode* ndoe, bool deep);
	static void alignLayout(CCNode* node, CCRect& rect,int align, int valign);
	static void margin(CCValue& v, float* top, float* left, float* bottom, float* right);
	static CCValue margin(float top, float left, float bottom, float right);
};

#endif // __CCE_APPUTIL_H_


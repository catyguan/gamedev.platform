#ifndef  __CCE_LAYOUT_UTIL_H__
#define  __CCE_LAYOUT_UTIL_H__

#include "cocos2d.h"

USING_NS_CC;

struct LayoutItem
{
	CCNode* node;
	int align;		// -1:none, 0-left, 1-middle, 2-right
	int valign;		// -1:none, 0-top, 1-center, 2-bottom
	int order;		// default:0
	std::string width;
	std::string height;
};
class LayoutUtil
{
public:
	static bool createLayoutItem(CCNode* node, LayoutItem& item);

	static void layout(CCNode* ndoe, bool deep);
	static void doLayout(CCNode* node, bool deep);
	static void doLayout(CCRect& rect, LayoutItem& item);
	static void alignLayout(CCNode* node, CCRect& rect,int align, int valign);
	static void resizeLayout(CCNode* node, CCRect& rect, std::string width, std::string height);
	static void margin(CCValue& v, float* top, float* left, float* bottom, float* right);
	static CCValue margin(float top, float left, float bottom, float right);

	static bool parseSize(std::string& str, int* percent, int* value);
};

#endif // __CCE_APPUTIL_H_


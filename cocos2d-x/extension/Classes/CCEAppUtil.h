#ifndef  __CCE_APPUTIL_H__
#define  __CCE_APPUTIL_H__

#include "cocos2d.h"

typedef struct tagResource
{
	std::string directory;
    cocos2d::CCSize size;    
} Resource;

typedef struct _CCEAppConfig
{
	std::string appId;
	int designWidth;
	int designHeight;
	int designPolicy;
} CCEAppConfig;

class CCEAppUtil
{
public:    
	static void readConfig(CCEAppConfig* cfg);
	static bool writeConfig(CCEAppConfig* cfg);

	static void initViewResolution(cocos2d::CCEGLView* view, Resource* resources, cocos2d::CCSize designResolutionSize);

	static cocos2d::CCRect getVisibleRect();
    static cocos2d::CCPoint getVisibleLeft();
    static cocos2d::CCPoint getVisibleRight();
    static cocos2d::CCPoint getVisibleTop();
    static cocos2d::CCPoint getVisibleBottom();
    static cocos2d::CCPoint getVisibleCenter();
    static cocos2d::CCPoint getVisibleLeftTop();
    static cocos2d::CCPoint getVisibleRightTop();
    static cocos2d::CCPoint getVisibleLeftBottom();
    static cocos2d::CCPoint getVisibleRightBottom();

	static long timestamp();
	static long timeTick();

private:
    static void lazyInit();
    static cocos2d::CCRect s_visibleRect;
};

#endif // __CCE_APPUTIL_H_


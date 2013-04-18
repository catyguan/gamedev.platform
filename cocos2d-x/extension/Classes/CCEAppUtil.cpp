#include "CCEAppUtil.h"

USING_NS_CC;

void CCEAppUtil::initViewResolution(cocos2d::CCEGLView* view, Resource* resources, CCSize designResolutionSize)
{
	CCSize frameSize = view->getFrameSize();
	float h = frameSize.height;
	if(frameSize.height>frameSize.width) {
		// ÊúÆÁ
		h = frameSize.width;
		view->setDesignResolutionSize(designResolutionSize.height, designResolutionSize.width, kResolutionNoBorder);
	} else {
		// Set the design resolution
		view->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionNoBorder);
	}

    // In this demo, we select resource according to the frame's height.
    // If the resource size is different from design resolution size, you need to set contentScaleFactor.
    // We use the ratio of resource's height to the height of design resolution,
    // this can make sure that the resource's height could fit for the height of design resolution.

	while(strlen(resources->directory)!=0) {
		// if the frame's height is larger than the height of medium resource size, select large resource.
		if (h >= resources->size.height)
		{ 
			CCLOG("designSize => %f x %f resource => %s", resources->size.width, resources->size.height, resources->directory);			
			CCFileUtils::sharedFileUtils()->addSearchPath(resources->directory);
			CCDirector::sharedDirector()->setContentScaleFactor(MIN(resources->size.height/designResolutionSize.height, resources->size.width/designResolutionSize.width));
			break;
		}
		resources++;
    }
}

CCRect CCEAppUtil::s_visibleRect;

void CCEAppUtil::lazyInit()
{
    if (s_visibleRect.size.width == 0.0f && s_visibleRect.size.height == 0.0f)
    {
        CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
        s_visibleRect.origin = pEGLView->getVisibleOrigin();
        s_visibleRect.size = pEGLView->getVisibleSize();
    }
}

CCRect CCEAppUtil::getVisibleRect()
{
    lazyInit();
    return CCRectMake(s_visibleRect.origin.x, s_visibleRect.origin.y, s_visibleRect.size.width, s_visibleRect.size.height);
}

CCPoint CCEAppUtil::getVisibleLeft()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x, s_visibleRect.origin.y+s_visibleRect.size.height/2);
}

CCPoint CCEAppUtil::getVisibleRight()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x+s_visibleRect.size.width, s_visibleRect.origin.y+s_visibleRect.size.height/2);
}

CCPoint CCEAppUtil::getVisibleTop()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x+s_visibleRect.size.width/2, s_visibleRect.origin.y+s_visibleRect.size.height);
}

CCPoint CCEAppUtil::getVisibleBottom()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x+s_visibleRect.size.width/2, s_visibleRect.origin.y);
}

CCPoint CCEAppUtil::getVisibleCenter()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x+s_visibleRect.size.width/2, s_visibleRect.origin.y+s_visibleRect.size.height/2);
}

CCPoint CCEAppUtil::getVisibleLeftTop()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x, s_visibleRect.origin.y+s_visibleRect.size.height);
}

CCPoint CCEAppUtil::getVisibleRightTop()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x+s_visibleRect.size.width, s_visibleRect.origin.y+s_visibleRect.size.height);
}

CCPoint CCEAppUtil::getVisibleLeftBottom()
{
    lazyInit();
    return s_visibleRect.origin;
}

CCPoint CCEAppUtil::getVisibleRightBottom()
{
    lazyInit();
    return ccp(s_visibleRect.origin.x+s_visibleRect.size.width, s_visibleRect.origin.y);
}

long CCEAppUtil::timestamp()
{
	return (long) time(NULL);
}

long CCEAppUtil::timeTick()
{
#ifndef CC_PLATFORM_WIN32
  struct timeval tp = {0};
  if (gettimeofday(&tp, NULL)) {
    return 0;
  } else {
	return (long)(tp.tv_sec*1000 + tp.tv_usec/1000);
  }  
#endif

#ifdef CC_PLATFORM_WIN32
  DWORD tm = GetTickCount();
  return (long)tm;
#endif
}
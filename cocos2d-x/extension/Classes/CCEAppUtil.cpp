#include "CCEAppUtil.h"
#include "CCEConfig.h"
#include "platform\CCFileSystem.h"
#include "CCEUtil.h"

USING_NS_CC;

void CCEAppUtil::readConfig(CCEAppConfig* cfg)
{
	cfg->appId = CCEConfig::get("appid").stringValue();
	cfg->designWidth = CCEConfig::get("design_width").intValue();
	cfg->designHeight = CCEConfig::get("design_height").intValue();
	cfg->designPolicy = CCEConfig::get("design_policy").intValue();

	std::string str = CCFileSystem::sharedFileSystem()->fileReadString(kAppData, "app.ini");
	std::map<std::string, std::string> props;
	StringUtil::parseProperties(str, "\n", "=", &props);

	std::map<std::string, std::string>::const_iterator it = props.begin();
	for(;it!=props.end();it++) {
		std::string key = it->first;
		std::string val = it->second;

		if(key.compare("appid")==0) {
			cfg->appId = val;
		} else if(key.compare("design_width")==0) {
			cfg->designWidth = atoi(val.c_str());
		} else if(key.compare("design_height")==0) {
			cfg->designHeight = atoi(val.c_str());
		} else if(key.compare("design_policy")==0) {
			cfg->designPolicy = atoi(val.c_str());
		}
	}
}

bool CCEAppUtil::writeConfig(CCEAppConfig* cfg)
{
	std::string content = "";
	content += StringUtil::format("appid=%s\n",cfg->appId.c_str());
	content += StringUtil::format("design_width=%d\n", cfg->designWidth);
	content += StringUtil::format("design_height=%d\n", cfg->designHeight);
	content += StringUtil::format("design_policy=%d\n", cfg->designPolicy);
	return CCFileSystem::sharedFileSystem()->fileWriteString(kAppData, "app.ini", content);
}

void CCEAppUtil::initViewResolution(cocos2d::CCEGLView* view, Resource* resources, CCSize designResolutionSize)
{
	CCSize frameSize = view->getFrameSize();
	float h = frameSize.height;
	CCLOG("frameSize => %f x %f, designSize => %f x %f",frameSize.width, frameSize.height, designResolutionSize.width, designResolutionSize.height);
	if(frameSize.height>frameSize.width) {
		// ����
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

	while(resources->directory.size()!=0) {
		// if the frame's height is larger than the height of medium resource size, select large resource.
		if (h >= resources->size.height)
		{ 
			float scaleFactor = MIN(resources->size.height/designResolutionSize.height, resources->size.width/designResolutionSize.width);
			CCLOG("resourceSize => %f x %f, resource => %s, scaleFactor => %f", resources->size.width, resources->size.height, resources->directory.c_str(), scaleFactor);			
			CCFileUtils::sharedFileUtils()->addSearchPath(resources->directory.c_str());
			CCDirector::sharedDirector()->setContentScaleFactor(scaleFactor);
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
#if CC_TARGET_PLATFORM!=CC_PLATFORM_WIN32
  struct timeval tp = {0};
  if (gettimeofday(&tp, NULL)) {
    return 0;
  } else {
	return (long)(tp.tv_sec*1000 + tp.tv_usec/1000);
  }  
#endif

#if CC_TARGET_PLATFORM==CC_PLATFORM_WIN32
  DWORD tm = GetTickCount();
  return (long)tm;
#endif
}
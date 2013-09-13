#ifndef __APPMACROS_H__
#define __APPMACROS_H__

#include "cocos2d.h"

// The font size 24 is designed for small resolution, so we should change it to fit for current design resolution
#define APP_FONT_SIZE(x)  (cocos2d::CCEGLView::sharedOpenGLView()->getDesignResolutionSize().width / 480 * x)
#define TITLE_FONT_SIZE  APP_FONT_SIZE(24)

#endif /* __APPMACROS_H__ */

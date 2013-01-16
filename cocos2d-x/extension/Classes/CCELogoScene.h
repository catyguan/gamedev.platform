#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "cocos2d.h"
#include "CCELayerTouch.h"

USING_NS_CC;

class CCELogoLayer : public CCELayerTouch
{
public:
	CCELogoLayer();
	virtual ~CCELogoLayer();

public:
	virtual bool init();  
	
	static CCELogoLayer* create(CCValueArray& ps);

	void clickHandler(CCNode* node, int type, CCTouch*);

protected:
	CCValue m_callDone;
};

#endif // __HELLOWORLD_SCENE_H__

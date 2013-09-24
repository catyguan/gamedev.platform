#ifndef __DEV_APP_CONFIG_SCENE_H__
#define __DEV_APP_CONFIG_SCENE_H__

#include "cocos2d.h"
#include "CCEScene.h"

USING_NS_CC;

class CCEDEVAppConfigScene : public CCEScene
{
public:
	CCEDEVAppConfigScene();
	virtual ~CCEDEVAppConfigScene();

public:
	virtual bool init();
	static CCEDEVAppConfigScene* create();
	
	void onBackClick(CCNode* node, const char* name, CCNodeEvent*);

	void onAppIdChanged(CCNode* node, const char* name, CCNodeEvent*);
	void onDesignWidthChanged(CCNode* node, const char* name, CCNodeEvent*);
	void onDesignHeightChanged(CCNode* node, const char* name, CCNodeEvent*);
	void onDesignPolicyChanged(CCNode* node, const char* name, CCNodeEvent*);

protected:
	
};

#endif // __HELLOWORLD_SCENE_H__

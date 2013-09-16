#ifndef __DEV_UPGRADE_CONFIG_SCENE_H__
#define __DEV_UPGRADE_CONFIG_SCENE_H__

#include "cocos2d.h"
#include "CCEScene.h"

USING_NS_CC;

class CCEDEVUpgradeConfigScene : public CCEScene
{
public:
	CCEDEVUpgradeConfigScene();
	virtual ~CCEDEVUpgradeConfigScene();

public:
	virtual bool init();
	static CCEDEVUpgradeConfigScene* create();
	
	void onBackClick(CCNode* node, const char* name, CCNodeEvent*);

	void onUrlChanged(CCNode* node, const char* name, CCNodeEvent*);
	void onHostChanged(CCNode* node, const char* name, CCNodeEvent*);
	void onVersionChanged(CCNode* node, const char* name, CCNodeEvent*);

protected:
	
};

#endif // __HELLOWORLD_SCENE_H__

#ifndef __DEV_UPGRADE_SCENE_H__
#define __DEV_UPGRADE_SCENE_H__

#include "cocos2d.h"
#include "CCEScene.h"
#include "CCEUpgradeManager.h"

USING_NS_CC;

class CCEDEVUpgradeScene : public CCEScene
{
public:
	CCEDEVUpgradeScene();
	virtual ~CCEDEVUpgradeScene();

public:
	virtual bool init();
	static CCEDEVUpgradeScene* create();

	virtual void update(float v);

	void onBackClick(CCNode* node, const char* name, CCNodeEvent*);
	void onGradeClick(CCNode* node, const char* name, CCNodeEvent*);

protected:
	UpgradeInfo m_lastInfo;
};

#endif // __HELLOWORLD_SCENE_H__

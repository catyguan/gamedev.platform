#ifndef __DEV_SCENE_H__
#define __DEV_SCENE_H__

#include "cocos2d.h"
#include "CCERootScene.h"

USING_NS_CC;

class CCEDEVScene : public CCERootScene
{
public:
	CCEDEVScene();
	virtual ~CCEDEVScene();

public:
	static CCEDEVScene* create(fnAppStartup fn, void* data);

	void menuCallback(CCObject* pSender); 

protected:

};

#endif // __HELLOWORLD_SCENE_H__

#ifndef __ROOT_SCENE_H__
#define __ROOT_SCENE_H__

#include "cocos2d.h"
#include "CCEScene.h"

USING_NS_CC;

typedef void (CALLBACK *fnAppStartup)(void* data);
class CCERootScene : public CCEScene
{
public:
	CCERootScene();
	virtual ~CCERootScene();

public:
	virtual bool init(fnAppStartup fn, void* data);

protected:
	void startupApp() {
		if(m_fnAppStartup!=NULL){
			m_fnAppStartup(m_appStartupData);
		}
	}

protected:
	fnAppStartup m_fnAppStartup;
	void* m_appStartupData;
};

#endif // __HELLOWORLD_SCENE_H__

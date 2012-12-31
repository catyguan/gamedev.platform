#ifndef  __CCE_SCENE_H__
#define  __CCE_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEActor;
class CCEScene : public cocos2d::CCScene
{
public:
	CCEScene();
	virtual ~CCEScene();

public:
	void setEnterCall(const char* fun, CCValueArray ps){enterFunction = fun;enterParams = ps;};

	void addActor(const char* name, CCEActor* actor);
	CCEActor* getActor(const char* name);	
	void removeActor(const char* name);
	virtual CCEActor* createActor(const char* name,CCValueArray& ps);

public:
	virtual bool init();
	virtual void onEnter();
	virtual void cleanup();	

protected:
	std::string enterFunction;
	CCValueArray enterParams;
	cocos2d::CCDictionary actors;
};

#endif
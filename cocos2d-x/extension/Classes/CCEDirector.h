#ifndef  __CCE_DIRECTOR_H__
#define  __CCE_DIRECTOR_H__

#include "cocos2d.h"

USING_NS_CC;

class CCELuaApplication;
class CCEDirector : public CCObject
{
public:
	CCEDirector(CCELuaApplication* app);
	virtual ~CCEDirector(void);

public:
	CCValue apiCall(std::string name, CCValueArray& ps);

	bool apiStartup();
	void apiShutdown();
	CCValue apiObjectCall(std::string name, CCValueArray& ps);
	CCValue apiObjectCall(std::string name);
	CCValue apiObjectCall(std::string name, CCValue p1);
	CCValue apiObjectCall(std::string name, CCValue p1, CCValue p2);
	CCValue apiObjectCall(std::string name, CCValue p1, CCValue p2, CCValue p3);

public:
	void install();
    static CCEDirector* sharedDirector(void);
    static void purgeSharedDirector(void);

protected:
	CCObject* buildObject(CCValue& cfg);
	CCAction* buildAction(CCValue& cfg);

protected:
	CCScene* m_scene;
	CCELuaApplication* m_app;

	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(winSize)
	CC_DECLARE_CALL(createObject)
	CC_DECLARE_CALL(buildObject)
	CC_DECLARE_CALL(buildAction)
	CC_DECLARE_CALL(scene)
	CC_DECLARE_CALL(pushScene)
	CC_DECLARE_CALL(replaceScene)
	CC_DECLARE_CALL(popScene)
	CC_DECLARE_CALLS_END	
};

#endif  // __CCE_DIRECTOR_H__
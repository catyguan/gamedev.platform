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

	void pause(CCObject* obj, bool children);
	void resume(CCObject* obj, bool children);
	void layout(CCNode* node, bool deep);	

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
	CC_DECLARE_CALL(layout)
	CC_DECLARE_CALL(createObject)
	CC_DECLARE_CALL(buildObject)
	CC_DECLARE_CALL(buildAction)
	CC_DECLARE_CALL(scene)
	CC_DECLARE_CALL(runScene)
	CC_DECLARE_CALL(pushScene)
	CC_DECLARE_CALL(replaceScene)
	CC_DECLARE_CALL(popScene)
	CC_DECLARE_CALL(contentScaleFactor)
	CC_DECLARE_CALL(pause)
	CC_DECLARE_CALL(resume)
	CC_DECLARE_CALLS_END
	// end_cc_call
};

#endif  // __CCE_DIRECTOR_H__
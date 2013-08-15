#ifndef __CCE_SCENE_H__
#define __CCE_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEScene : public CCScene
{
public:
    CCEScene();
    virtual ~CCEScene();

public:
	void update4nextFrame(float d);
	
public:
	virtual bool init();
	static CCEScene* create();

protected:
	CCValueArray m_nextFrameCall;

	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(nextFrame)
	CC_DECLARE_CALLS_END
};

#endif
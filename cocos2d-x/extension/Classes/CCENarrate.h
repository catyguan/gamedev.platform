#ifndef __CCE_NARRATE_H__
#define __CCE_NARRATE_H__

#include "cocos2d.h"

USING_NS_CC;

// how many MS to display a line(page)
#define NARRATE_DEFAULT_SPEED	0.75

#define NODE_EVENT_NARRATE_PAUSE	"narratePause"
#define NODE_EVENT_NARRATE_END		"narrateEnd"

class CCENarrate: public CCNodeRGBA
{
public:
    CCENarrate();
    virtual ~CCENarrate();

public:
	virtual void cleanup();
	
public:
	virtual bool init(CCLabelTTF* label, int width, int height, float speed);
	static CCENarrate* create(CCLabelTTF* label, int width, int height, float speed);

	virtual bool init();
	static CCENarrate* create();

	CCLabelTTF* getLabel();
	void setLabel(CCLabelTTF* v);
	float getSpeed();
	void setSpeed(float v);

	virtual bool setup(CCValue& cfg);
	virtual void setContentSize(CCSize& sz);

	void build();

	bool process(CCValue callback);
	void fastForward();
	bool isEnd();

	// cc_call
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(label)
	CC_DECLARE_CALL(speed)
	CC_DECLARE_CALL(build)
	CC_DECLARE_CALL(process)
	CC_DECLARE_CALL(isEnd)
	CC_DECLARE_CALL(updateNarrate)
	CC_DECLARE_CALL(pauseNarrate)
	CC_DECLARE_CALL(endNarrate)
	CC_DECLARE_CALL(fastForward)
	CC_DECLARE_CALLS_END
	// end cc_call

protected:
	CCAction* nextPageShowAction();		
	void updateNarrate(float t);
	void pauseNarrate();
	void endNarrate();
	void showLine(int line, int tag, float width);

protected:
	CCLabelTTF* m_label;
	
	float m_speed;

	int m_lineNum;
	int m_lineHeight;
	int m_pageLine;
	int m_pageNum;

	float m_duration;
	int m_currentPage;
	int m_currentLine;

	CCValue m_callback;
};

#endif
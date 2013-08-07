#ifndef __CCE_NARRATE_H__
#define __CCE_NARRATE_H__

#include "cocos2d.h"

USING_NS_CC;

#define NARRATE_MODE_WORD		0
#define NARRATE_MODE_LINE		1
#define NARRATE_MODE_PAGE		2

// how many MS to display a line(page)
#define NARRATE_DEFAULT_SPEED	0.5

class CCENarrate: public CCNodeRGBA
{
public:
    CCENarrate();
    virtual ~CCENarrate();

public:
	virtual void cleanup();
	
public:
	virtual bool init(CCLabelTTF* label, int width, int height, int mode, float speed);
	static CCENarrate* create(CCLabelTTF* label, int width, int height,int mode, float speed);

	float getPagePauseTime();
	void setPagePauseTime(float t);

	void build();

	CCAction* createAction();	

	bool process(CCValue callback);
	bool isEnd();
	bool isPause();

	// cc_call
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(pagePauseTime)
	CC_DECLARE_CALL(process)
	CC_DECLARE_CALL(isEnd)
	CC_DECLARE_CALL(isPause)
	CC_DECLARE_CALL(updateNarrate)
	CC_DECLARE_CALL(pauseNarrate)
	CC_DECLARE_CALL(endNarrate)
	CC_DECLARE_CALLS_END
	// end cc_call

protected:
	void updateNarrate(float t, float duration, float basetime);
	void showLine(int line, int tag, float width);

protected:
	CCLabelTTF* m_label;
	
	int m_mode;
	float m_speed;
	float m_pagePauseTime;

	int m_lineNum;
	int m_lineHeight;
	int m_pageLine;
	int m_pageNum;

	int m_currentLine;
	int m_currentPage;

	CCValue m_callback;
};

#endif
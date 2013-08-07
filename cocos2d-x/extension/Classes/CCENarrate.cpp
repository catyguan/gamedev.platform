#include "CCENarrate.h"
#include "cocoa\CCValueSupport.h"
#include "actions\CCAction.h"
#include "CCEAction.h"

// CCENarrate
CCENarrate::CCENarrate()
{
	m_label = NULL;
}

CCENarrate::~CCENarrate()
{
	CC_SAFE_RELEASE_NULL(m_label);
}

void CCENarrate::cleanup()
{
	CCNodeRGBA::cleanup();
	m_callback.cleanup();
	CC_SAFE_RELEASE_NULL(m_label);
}

bool CCENarrate::init(CCLabelTTF* label, int width, int height, int mode, float speed)
{
	CC_ASSERT(label);
	CC_ASSERT(height>0);

	if(!CCNodeRGBA::init()) {
		return false;
	}
	m_label = label;
	CC_SAFE_RETAIN(m_label);
	setContentSize(CCSizeMake(width, height));
	m_mode = mode;
	m_speed = (float) (speed>0?speed:NARRATE_DEFAULT_SPEED);
	m_pagePauseTime = -1;
	
	build();

	return true;
}

CCENarrate* CCENarrate::create(CCLabelTTF* label, int width, int height, int mode, float speed)
{	
    CCENarrate *pRet = new CCENarrate();
	if (pRet && pRet->init(label, width, height, mode, speed))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

float CCENarrate::getPagePauseTime()
{
	return m_pagePauseTime;
}

void CCENarrate::setPagePauseTime(float t)
{
	m_pagePauseTime = t;
}

void CCENarrate::build()
{
	CC_ASSERT(m_label);
	m_lineNum = 1;
	m_lineHeight = 0;
	m_pageNum = 0;
	m_pageLine = 0;
	m_currentLine = 1;
	m_currentPage = 1;

	CCSize labelSize = m_label->getContentSize();
	std::string str = m_label->getString();
	CCSize mySize = getContentSize();
	
	std::string::size_type pos = 0;
	while((pos=str.find_first_of('\n',pos))!=std::string::npos)  
	{  
		m_lineNum++;
		pos++;		
	}
	m_lineHeight = (int) labelSize.height/m_lineNum;
	m_pageLine = (int) mySize.height/m_lineHeight;
	if(m_pageLine<0)m_pageLine = 1;
	m_pageNum = (int) ceil(m_lineNum*1.0/m_pageLine);

	for(int i=1;i<=m_pageLine;i++) {	
		CCSprite* sp = CCSprite::create();
		sp->setAnchorPoint(CCPointZero);
		sp->setPosition(CCPointMake(0, mySize.height-i*m_lineHeight));
		sp->setVisible(false);
		addChild(sp,0,i);
	}
}

CCAction* CCENarrate::createAction()
{	
	if(m_pagePauseTime==0) {
		float d = (float) (m_speed*m_lineNum);
		CCFiniteTimeAction* a1 = CCECallInterval::create(d, this, "updateNarrate", CCValue::numberValue(d), CCValue::numberValue(0));
		CCFiniteTimeAction* a2 = CCECall::create(this, "endNarrate");
		return CCSequence::createWithTwoActions(a1,a2);
	} else if(m_pagePauseTime>0) {
		CCArray* a = CCArray::create();
		float pd = m_speed*m_pageLine;
		for(int i=0;i<m_pageNum;i++) {			
			float d = (float)((i==m_pageNum-1)?(m_speed*(m_lineNum-i*m_pageLine)):pd);
			float bt = (float) (pd*i);
			CCECallInterval* a1 = CCECallInterval::create(d, this, "updateNarrate", CCValue::numberValue(d), CCValue::numberValue(bt));
			a->addObject(a1);			
			a->addObject(CCDelayTime::create(m_pagePauseTime));
		}
		a->addObject(CCECall::create(this, "endNarrate"));
		return CCSequence::create(a);		
	} else {

	}
	return NULL;
}

void CCENarrate::showLine(int line, int tag, float width)
{
	// CCLOG("showLine - %d,%d,%f", line, tag, width);
	CCSprite* sp = (CCSprite*) getChildByTag(tag);
	CC_ASSERT(sp);
	int h = (line-1)*m_lineHeight;
	CCSize sz = m_label->getContentSize();
	sp->initWithTexture(m_label->getTexture(), CCRectMake(0,h,width,m_lineHeight));
	sp->setAnchorPoint(CCPointZero);
	sp->setVisible(true);
}

void CCENarrate::updateNarrate(float t, float duration, float basetime)
{
	if(isEnd())return;

	t = t*duration+basetime;
	CCLOG("updateNarrate %f", t);
	if(t==0)return;
	int uline = (int) ceil(t/m_speed);
	if(uline==0)uline = 1;
	int upage = (int) ceil(uline*1.0/m_pageLine);

	if(upage>m_currentPage) {
		// page down
		m_currentPage = upage;
		if(isEnd())return;
		// CCLOG("page down %d", upage);
		for(int i=1;i<=m_pageLine;i++) {
			CCNode* node = getChildByTag(i);
			if(node!=NULL)node->setVisible(false);
		}
	}
	float width = min(m_label->getContentSize().width, getContentSize().width);
	float lt = m_speed;
	int pagehold = (m_currentPage-1)*m_pageLine;
	for(int l=m_currentLine;l<=uline;l++) {
		if(l<=pagehold)continue;
		float w;
		float ts = (l-1)*lt;
		float tw = (t-ts)/m_speed;
		if(tw>1) {
			w = width;
		} else {
			w = (int) width*tw;
		}
		int tag = (l-1)%m_pageLine+1;
		showLine(l,tag,w);
	}
	m_currentLine = uline;
}

bool CCENarrate::process(CCValue callback)
{
	if(isEnd())return false;
	m_callback.cleanup();
	m_callback = callback;
	m_callback.retain();
	runAction(createAction());
	return true;
}

bool CCENarrate::isEnd()
{	
	return m_currentPage>m_pageNum;
}

bool CCENarrate::isPause()
{
	return false;
}

// cc_call
CC_BEGIN_CALLS(CCENarrate, CCNodeRGBA)
	CC_DEFINE_CALL(CCENarrate, updateNarrate)
	CC_DEFINE_CALL(CCENarrate, pauseNarrate)
	CC_DEFINE_CALL(CCENarrate, endNarrate)
	CC_DEFINE_CALL(CCENarrate, pagePauseTime)
	CC_DEFINE_CALL(CCENarrate, process)
	CC_DEFINE_CALL(CCENarrate, isEnd)
	CC_DEFINE_CALL(CCENarrate, isPause)
CC_END_CALLS(CCENarrate, CCNodeRGBA)

CCValue CCENarrate::CALLNAME(pagePauseTime)(CCValueArray& params) {	
	if(params.size()>0) {
		float t = ccvpFloat(params, 0);
		setPagePauseTime(t);
	}
	return CCValue::numberValue(getPagePauseTime());
}
CCValue CCENarrate::CALLNAME(updateNarrate)(CCValueArray& params) {		
	float d = ccvpFloat(params,0);
	float bt = ccvpFloat(params, 1);
	float t = ccvpFloat(params, 2);
	updateNarrate(t,d,bt);
	return CCValue::nullValue();
}
CCValue CCENarrate::CALLNAME(pauseNarrate)(CCValueArray& params) {		
	m_currentLine = m_pageNum+1;
	return CCValue::nullValue();
}
CCValue CCENarrate::CALLNAME(endNarrate)(CCValueArray& params) {		
	m_currentLine = m_pageNum+1;
	if(m_callback.canCall()) {
		CCValueArray ps;
		ps.push_back(CCValue::booleanValue(true));
		m_callback.call(ps,false);
	}
	return CCValue::nullValue();
}
CCValue CCENarrate::CALLNAME(process)(CCValueArray& params) {	
	CCValue v;
	if(params.size()>0) {
		v = params[0];
	}
	return CCValue::booleanValue(process(v));
}
CCValue CCENarrate::CALLNAME(isEnd)(CCValueArray& params) {
	return CCValue::booleanValue(isEnd());
}
CCValue CCENarrate::CALLNAME(isPause)(CCValueArray& params) {
	return CCValue::booleanValue(isPause());
}
// end cc_call
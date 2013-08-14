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

CCLabelTTF* CCENarrate::getLabel()
{
	return m_label;
}

void CCENarrate::setLabel(CCLabelTTF* v)
{
	if(v!=NULL) {
		CC_SAFE_RELEASE(m_label);
		m_label = v;
		CC_SAFE_RETAIN(m_label);
	}
}

float CCENarrate::getSpeed()
{
	return m_speed;
}

void CCENarrate::setSpeed(float v)
{
	m_speed = v;
}

void CCENarrate::setContentSize(CCSize& sz)
{
	CCNodeRGBA::setContentSize(sz);
	build();
}
void CCENarrate::cleanup()
{
	CCNodeRGBA::cleanup();
	m_callback.cleanup();
	CC_SAFE_RELEASE_NULL(m_label);
}

bool CCENarrate::init()
{
	if(!CCNodeRGBA::init()) {
		return false;
	}
	return true;
}

bool CCENarrate::init(CCLabelTTF* label, int width, int height, float speed)
{
	CC_ASSERT(label);
	CC_ASSERT(height>0);

	if(!init()) {
		return false;
	}
	m_label = label;
	CC_SAFE_RETAIN(m_label);	
	m_speed = (float) (speed>0?speed:NARRATE_DEFAULT_SPEED);
	setContentSize(CCSizeMake(width, height));

	return true;
}

CCENarrate* CCENarrate::create(CCLabelTTF* label, int width, int height, float speed)
{	
    CCENarrate *pRet = new CCENarrate();
	if (pRet && pRet->init(label, width, height, speed))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

CCENarrate* CCENarrate::create()
{	
    CCENarrate *pRet = new CCENarrate();
	if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool CCENarrate::setup(CCValue& cfg)
{
	if(!CCNodeRGBA::setup(cfg)) {
		return false;
	}
	build();
	return true;
}

void CCENarrate::build()
{
	if(m_label==NULL)return;
	CCSize mySize = getContentSize();
	if(mySize.width==0 || mySize.height==0)return;

	CC_ASSERT(m_label);
	m_lineNum = 1;
	m_lineHeight = 0;
	m_pageNum = 0;
	m_pageLine = 0;
	m_currentLine = 0;
	m_currentPage = 0;
	m_duration = 0;

	CCSize labelSize = m_label->getContentSize();
	std::string str = m_label->getString();
	
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

	removeAllChildren();
	for(int i=1;i<=m_pageLine;i++) {	
		CCSprite* sp = CCSprite::create();
		sp->setAnchorPoint(CCPointZero);
		sp->setPosition(CCPointMake(0, mySize.height-i*m_lineHeight));
		sp->setVisible(false);
		addChild(sp,0,i);
	}
}

CCAction* CCENarrate::nextPageShowAction()
{	
	m_currentPage++;
	m_currentLine=1;
	m_duration = 0;

	if(isEnd()) {
		return CCECall::create(this, "endNarrate");
	} else {
		for(int i=1;i<=m_pageLine;i++) {
			CCNode* node = getChildByTag(i);
			if(node!=NULL)node->setVisible(false);
		}
		float d = 0;
		if(m_currentPage==m_pageNum) {
			d = (float) (m_speed*(m_lineNum-(m_currentPage-1)*m_pageLine));
		} else {
			d = (float) (m_speed*m_pageLine);
		}
		m_duration = d;
		CCFiniteTimeAction* a1 = CCECallInterval::create(d, this, "updateNarrate");
		CCFiniteTimeAction* a2 = CCECall::create(this, "pauseNarrate");
		return CCSequence::createWithTwoActions(a1,a2);
	}
}

void CCENarrate::showLine(int line, int tag, float width)
{
	// CCLOG("showLine - %d,%d,%f", line, tag, width);
	CCSprite* sp = (CCSprite*) getChildByTag(tag);
	CC_ASSERT(sp);
	int h = (line-1)*m_lineHeight;
	sp->initWithTexture(m_label->getTexture(), CCRectMake(0,h,width,m_lineHeight));
	sp->setAnchorPoint(CCPointZero);
	sp->setVisible(true);
}

void CCENarrate::updateNarrate(float t)
{
	if(isEnd())return;

	t = t*m_duration;
	// CCLOG("updateNarrate %f", t);
	if(t==0)return;
	int uline = (int) ceil(t/m_speed);
	if(uline==0)uline = 1;

	CCSize labelSize = m_label->getContentSize();
	float width = min(labelSize.width, getContentSize().width);
	float lt = m_speed;
	
	int linebase = (m_currentPage-1)*m_pageLine;
	for(int l=m_currentLine;l<=uline;l++) {
		float w;
		float ts = (l-1)*lt;
		float tw = (t-ts)/m_speed;
		if(tw>1) {
			w = width;
		} else {
			w = (int) width*tw;
		}
		int tag = (l-1)%m_pageLine+1;
		showLine(l+linebase,tag,w);
	}
	m_currentLine = uline;
}

void CCENarrate::fastForward()
{
	this->stopAllActions();
	if(isEnd()) {
		endNarrate();
		return;
	}

	updateNarrate(1);
	CCAction* act = CCECall::create(this,"pauseNarrate");	
	runAction(act);
}

void CCENarrate::pauseNarrate()
{
	if(m_callback.canCall()) {
		CCValueArray ps;
		ps.push_back(CCValue::booleanValue(false));
		m_callback.call(ps,false);
	} else {
		raiseEvent(NODE_EVENT_NARRATE_PAUSE,NULL);
	}
}

void CCENarrate::endNarrate()
{	
	m_currentLine = m_pageNum+1;
	if(m_callback.canCall()) {
		CCValueArray ps;
		ps.push_back(CCValue::booleanValue(true));
		m_callback.call(ps,false);
	} else {
		raiseEvent(NODE_EVENT_NARRATE_END, NULL);
	}
}

bool CCENarrate::process(CCValue callback)
{
	if(isEnd())return false;
	m_callback.cleanup();
	m_callback = callback;
	m_callback.retain();
	runAction(nextPageShowAction());
	return true;
}

bool CCENarrate::isEnd()
{	
	return m_currentPage>m_pageNum;
}

// cc_call
CC_BEGIN_CALLS(CCENarrate, CCNodeRGBA)
	CC_DEFINE_CALL(CCENarrate, label)
	CC_DEFINE_CALL(CCENarrate, speed)
	CC_DEFINE_CALL(CCENarrate, updateNarrate)
	CC_DEFINE_CALL(CCENarrate, pauseNarrate)
	CC_DEFINE_CALL(CCENarrate, endNarrate)
	CC_DEFINE_CALL(CCENarrate, build)
	CC_DEFINE_CALL(CCENarrate, process)
	CC_DEFINE_CALL(CCENarrate, isEnd)
CC_END_CALLS(CCENarrate, CCNodeRGBA)

CCValue CCENarrate::CALLNAME(updateNarrate)(CCValueArray& params) {		
	float t = ccvpFloat(params,0);
	updateNarrate(t);
	return CCValue::nullValue();
}
CCValue CCENarrate::CALLNAME(pauseNarrate)(CCValueArray& params) {		
	pauseNarrate();
	return CCValue::nullValue();
}
CCValue CCENarrate::CALLNAME(endNarrate)(CCValueArray& params) {		
	endNarrate();
	return CCValue::nullValue();
}
CCValue CCENarrate::CALLNAME(label)(CCValueArray& params) {		
	if(params.size()>0) {
		CCLabelTTF* obj = ccvpObject(params,0,CCLabelTTF);
		if(obj!=NULL) {
			setLabel(obj);
		}
	}
	return CCValue::objectValue(m_label);
}
CCValue CCENarrate::CALLNAME(speed)(CCValueArray& params) {		
	if(params.size()>0) {
		float v = params[0].floatValue();
		setSpeed(v);
	}
	return CCValue::numberValue(m_speed);
}
CCValue CCENarrate::CALLNAME(build)(CCValueArray& params) {	
	CCValue v;
	if(params.size()>0) {
		v = params[0];
	}
	build();
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
// end cc_call
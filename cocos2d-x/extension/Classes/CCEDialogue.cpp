#include "CCEDialogue.h"
#include "CCELayerTouch.h"
#include "CCENarrate.h"
#include "cocoa\CCValueSupport.h"
#include "base_nodes/CCNode_Events.h"
#include "CCELayoutUtil.h"

#define TAG_FRAME	1
#define TAG_PAUSE	2
#define TAG_NARRATE	3

struct DialogueInfo {
	// narrate
	float speed;
	float pauseTime;
	// label
	std::string content;
	// callback
	CCValue callback;
};

CCEDialogue::CCEDialogue()
{
	m_narrate = NULL;
	m_label = NULL;
	m_info = NULL;
}

CCEDialogue::~CCEDialogue()
{
    clear();
}

CCNode* CCEDialogue::getFrame()
{
	return getChildByTag(TAG_FRAME);
}

void CCEDialogue::setFrame(CCNode* node)
{
	if(getChildByTag(TAG_FRAME)!=NULL) {
		removeChildByTag(TAG_FRAME);
	}
	addChild(node,0,TAG_FRAME);
}

CCLabelTTF* CCEDialogue::getLabel()
{
	return m_label;
}
void CCEDialogue::setLabel(CCLabelTTF* label)
{
	if(label!=NULL) {
		CC_SAFE_RELEASE_NULL(m_label);
		m_label = label;
		CC_SAFE_RETAIN(m_label);
	}
}

CCNode* CCEDialogue::getPauseNode()
{
	return getChildByTag(TAG_PAUSE);
}

void CCEDialogue::setPauseNode(CCNode* node)
{
	if(getChildByTag(TAG_PAUSE)!=NULL) {
		removeChildByTag(TAG_PAUSE);
	}
	addChild(node,10,TAG_PAUSE);
}

CCSize CCEDialogue::getMaginTopLeft()
{
	return m_marginTopLeft;
}

void CCEDialogue::setMarginTopLeft(CCSize sz)
{
	m_marginTopLeft = sz;
}

CCSize CCEDialogue::getMarginBottomRight()
{
	return m_marginBottomRight;
}

void CCEDialogue::setMarginBottomRight(CCSize sz)
{
	m_marginBottomRight = sz;
}

void CCEDialogue::setContentSize(const CCSize& contentSize)
{
	CCLayer::setContentSize(contentSize);
	layout();
}

void CCEDialogue::onEnter()
{
	CCLayer::onEnter();

	CCELayerTouch* touch = CCELayerTouch::getTouchLayer(this);
	if(touch!=NULL) {
		CCETouchBuilder b;
		b.bind(this).onTap(this, nodeevent_selector(CCEDialogue::onPauseNodeClick)).createTouch(touch);		
	} else {
		CCLOG("CCEDialogue::onEnter - getTouchLayer is null");
	}	
}

void CCEDialogue::cleanup()
{
	CCLayer::cleanup();
	clear();
}

void CCEDialogue::clear()
{
	CC_SAFE_RELEASE_NULL(m_label);
	if(m_info!=NULL) {
		delete m_info;
		m_info = NULL;
	}
}

bool CCEDialogue::init()
{
	if(!CCLayer::init()) {
		return false;
	}
	setVisible(false);
	return true;
}

CCEDialogue* CCEDialogue::create()
{    
    CCEDialogue *pRet = new CCEDialogue();
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

void CCEDialogue::layout()
{
	CCSize size = getContentSize();
	CCNode* frame = getFrame();
	if(frame!=NULL) {
		frame->setAnchorPoint(CCPointZero);
		frame->setPosition(0,0);
		frame->setContentSize(size);		
	}

	if(m_narrate!=NULL) {
		float x1 = m_marginTopLeft.width;
		float y1 = m_marginTopLeft.height;
		float x2 = size.width - m_marginBottomRight.width;
		float y2 = size.height - m_marginBottomRight.height;

		CCPoint pt(x1,y1);
		m_narrate->setPosition(pt);
		CCSize sz(x2-x1,y2-y1);
		m_narrate->setContentSize(sz);
	}

	CCNode* pnode = getPauseNode();
	if(pnode!=NULL) {
		pnode->setVisible(false);
	}
}

void CCEDialogue::build(DialogueInfo* info, DialogueInfo* old)
{
	setVisible(false);

	if(m_label==NULL) {
		m_label = CCLabelTTF::create("", "Arial", 26);
		CC_SAFE_RETAIN(m_label);
	}
	m_label->setString(info->content.c_str());

	if(m_narrate==NULL) {
		m_narrate = CCENarrate::create();		
		addChild(m_narrate,1,TAG_NARRATE);

		m_narrate->onEvent(NODE_EVENT_NARRATE_PAUSE, this, nodeevent_selector(CCEDialogue::onNarratePause));
		m_narrate->onEvent(NODE_EVENT_NARRATE_END, this, nodeevent_selector(CCEDialogue::onNarrateEnd));
	}	
	m_narrate->setLabel(m_label);
	m_narrate->setSpeed(info->speed);
	layout();
	m_narrate->build();

	setVisible(true);
	m_narrate->process(CCValue::nullValue());
}

bool CCEDialogue::showDialogue(CCValue& properties, CCValue call)
{
	if(getParent()==NULL) {
		throw new std::string("add to scene before show dialogue");
	}

	CCValueReader r(&properties);
	DialogueInfo* info = new DialogueInfo();
	// init
	info->pauseTime = m_info!=NULL?m_info->pauseTime:-1;
	info->speed = m_info!=NULL?m_info->speed:0.75f;	

	// read
	CCValue* v;
	if(r.isMap()) {		
		v = r.getNull("pauseTime");
		if(v!=NULL)info->pauseTime = v->floatValue();

		v = r.getNull("speed");
		if(v!=NULL)info->speed = v->floatValue();

		v = r.getNull("text");
		if(v!=NULL)info->content = v->stringValue();
	}

	// buildup
	m_paused = false;

	DialogueInfo* old = m_info;
	m_info = info;
	m_info->callback = call;
	m_info->callback.retain();
	if(m_info->content.size()>0) {
		build(m_info,old);
	}
	CC_SAFE_DELETE(old);

	return true;
}

void CCEDialogue::pauseNarrate()
{
	m_paused = true;
	CCNode* node = getPauseNode();
	if(node!=NULL) {		
		node->setVisible(true);
		if(node->canCall("startAnimation")) {
			CCValueArray ps;
			node->call("startAnimation", ps);
		}
	}
}

void CCEDialogue::endNarrate()
{
	if(m_info->callback.canCall()) {
		CCValueArray ps;
		m_info->callback.call(ps, false);
	}
}

void CCEDialogue::nextPage()
{
	m_paused = false;
	CCNode* node = getPauseNode();
	if(node!=NULL) {
		if(node->canCall("stopAnimation")) {
			CCValueArray ps;
			node->call("stopAnimation", ps);
		} else {
			node->stopAllActions();
		}
		node->setVisible(false);
	}

	m_narrate->process(CCValue::nullValue());
}

void CCEDialogue::onPauseNodeClick(CCNode* node, const char* name, CCNodeEvent*)
{
	if(m_paused) {
		nextPage();
	} else {
		if(m_narrate!=NULL) {
			m_narrate->fastForward();
		}
	}
}

void CCEDialogue::onNarratePause(CCNode* node, const char* name, CCNodeEvent*)
{
	pauseNarrate();
}

void CCEDialogue::onNarrateEnd(CCNode* node, const char* name, CCNodeEvent*)
{
	endNarrate();
}

void CCEDialogue::updatePauseNode()
{
	
}

// cc_call
CC_BEGIN_CALLS(CCEDialogue, CCLayer)
	CC_DEFINE_CALL(CCEDialogue, label)
	CC_DEFINE_CALL(CCEDialogue, frame)
	CC_DEFINE_CALL(CCEDialogue, margin)
	CC_DEFINE_CALL(CCEDialogue, pauseNode)
	CC_DEFINE_CALL(CCEDialogue, show)	
CC_END_CALLS(CCEDialogue, CCLayer)

CCValue CCEDialogue::CALLNAME(label)(CCValueArray& params) {
	CCLabelTTF* node = ccvpObject(params,0,CCLabelTTF);
	if(node!=NULL) {
		setLabel(node);
	}
	return CCValue::objectValue(getLabel());
}
CCValue CCEDialogue::CALLNAME(frame)(CCValueArray& params) {
	CCNode* node = ccvpObject(params,0,CCNode);
	if(node!=NULL) {
		setFrame(node);
	}
	return CCValue::objectValue(getFrame());
}
CCValue CCEDialogue::CALLNAME(pauseNode)(CCValueArray& params) {
	CCNode* node = ccvpObject(params,0,CCNode);
	if(node!=NULL) {
		setPauseNode(node);
	}
	return CCValue::objectValue(getPauseNode());
}
CCValue CCEDialogue::CALLNAME(margin)(CCValueArray& params) {
	if(params.size()>0) {
		LayoutUtil::margin(params[0], 
			&m_marginTopLeft.width,
			&m_marginTopLeft.height,
			&m_marginBottomRight.width,
			&m_marginBottomRight.height);
		return CCValue::nullValue();
	} else {
		return LayoutUtil::margin( 
			m_marginTopLeft.width,
			m_marginTopLeft.height,
			m_marginBottomRight.width,
			m_marginBottomRight.height);
	}
}

CCValue nullValue;
CCValue CCEDialogue::CALLNAME(show)(CCValueArray& params) {	
	CCValue& v = params.size()>0?params.at(0):nullValue;
	showDialogue(v, ccvp(params, 1));
	return CCValue::nullValue();
}
// end cc_call
#include "CCETouch.h"
#include "CCDirector.h"

#include "cocoa\CCValueSupport.h"

// CCEEvents
CCValue CCETouchEvent::toValue()
{
	CCValueMap map;
	map["position"] = CCValueUtil::point(m_touch.x, m_touch.y);
	return CCValue::mapValue(map);
}


CCValue CCETouchFocusEvent::toValue()
{
	CCValueMap map;
	map["focus"] = CCValue::booleanValue(m_focus);
	return CCValue::mapValue(map);
}

CCValue CCETouchHoldEvent::toValue()
{
	CCValueMap map;
	map["hold"] = CCValue::booleanValue(m_hold);
	return CCValue::mapValue(map);
}

CCValue CCETouchPanEvent::toValue()
{
	CCValueMap map;
	map["from"] = CCValueUtil::point(m_from.x, m_from.y);
	return CCValue::mapValue(map);
}

//
// CCEGestureRecognizer
//
CCEGestureRecognizer::CCEGestureRecognizer()
{
}

CCEGestureRecognizer::~CCEGestureRecognizer()
{	
}

//
// CCEGestureRecognizer4Node
//
CCEGestureRecognizer4Node::CCEGestureRecognizer4Node()
	: m_node(NULL)
{
	
}

CCEGestureRecognizer4Node::~CCEGestureRecognizer4Node()
{
	CC_SAFE_RELEASE(m_node);
}

void CCEGestureRecognizer4Node::init(CCNode* node)
{
	CC_SAFE_RELEASE_NULL(m_node);
	if(node!=NULL) {
		m_node = node;
		CC_SAFE_RETAIN(m_node);
	}
}

//
// CCEGestureRecognizer4Chain
//
CCEGestureRecognizer4Chain::CCEGestureRecognizer4Chain()
{
	
}

CCEGestureRecognizer4Chain::~CCEGestureRecognizer4Chain()
{
	clear();
}

CCEGestureRecognizer4Chain* CCEGestureRecognizer4Chain::add(CCEGestureRecognizer* sub)
{
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		if((*it)==sub)return this;
	}
	m_chain.push_back(sub);
	return this;
}

bool CCEGestureRecognizer4Chain::remove(CCEGestureRecognizer* sub,bool deleteObj)
{
	std::list<CCEGestureRecognizer*>::iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		if((*it)==sub) {
			m_chain.erase(it);
			if(deleteObj) {
				delete sub;
			}
			return true;
		}
	}
	return false;
}

void CCEGestureRecognizer4Chain::clear()
{
	std::list<CCEGestureRecognizer*>::iterator it;
	while(m_chain.size()>0) {		
		it = m_chain.begin();
		CCEGestureRecognizer* ptr = *it;
		m_chain.erase(it);
		delete ptr;
	}
}

bool CCEGestureRecognizer4Chain::touchBegan(int id, CCPoint touch)
{
	bool r = false;
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		if((*it)->touchBegan(id, touch)) {
			r = true;
		}
	}
	return r;
}

bool CCEGestureRecognizer4Chain::touchMoved(int id, CCPoint touch)
{
	bool r = false;
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		if((*it)->touchMoved(id, touch)) {
			r = true;
		}
	}
	return r;
}

void CCEGestureRecognizer4Chain::touchCancelled(int id, CCPoint touch)
{
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		(*it)->touchCancelled(id, touch);
	}
}

bool CCEGestureRecognizer4Chain::touchEnded(int id, CCPoint touch)
{
	bool r = false;
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		if((*it)->touchEnded(id, touch)) {
			r = true;
		}
	}
	return r;
}

bool CCEGestureRecognizer4Chain::isRecognized()
{
	bool r = false;
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=m_chain.begin();it!=m_chain.end();it++) {
		if((*it)->isRecognized()) {
			r = true;
		}
	}
	return r;
}


//
// CCEGestureRecognizer4Focus
//
CCEGestureRecognizer4Focus::CCEGestureRecognizer4Focus()
	: m_focus(false)
{
	
}

CCEGestureRecognizer4Focus::~CCEGestureRecognizer4Focus()
{
	
}

CCEGestureRecognizer4Focus* CCEGestureRecognizer4Focus::create(CCNode* node)
{
	CCEGestureRecognizer4Focus* r = new CCEGestureRecognizer4Focus();
	r->init(node);
	return r;
}

void CCEGestureRecognizer4Focus::setFocus(CCPoint touch, bool v)
{
	if(m_focus!=v) {
		m_focus = v;
		CCETouchFocusEvent event(touch, m_focus);
		m_node->raiseEvent(NODE_EVENT_FOCUS,&event);	
	}
}

bool CCEGestureRecognizer4Focus::touchBegan(int id, CCPoint touch)
{
	return touchMoved(id, touch);
}

bool CCEGestureRecognizer4Focus::touchMoved(int id, CCPoint touch)
{
	CCPoint local = m_node->convertToNodeSpace(touch);
	if(m_node->containsPoint(local)) {
		setFocus(touch, true);
	} else {
		setFocus(touch, false);
	}
	return true;
}

void CCEGestureRecognizer4Focus::touchCancelled(int id, CCPoint touch)
{
	setFocus(touch, false);
}

bool CCEGestureRecognizer4Focus::touchEnded(int id, CCPoint touch)
{
	setFocus(touch,false);
	return false;
}

bool CCEGestureRecognizer4Focus::isRecognized()
{	
	return false;
}


//
// CCEGestureRecognizer4Tap
//
CCEGestureRecognizer4Tap::CCEGestureRecognizer4Tap()
	: m_tap(false)
{
	
}

CCEGestureRecognizer4Tap::~CCEGestureRecognizer4Tap()
{
	
}

CCEGestureRecognizer4Tap* CCEGestureRecognizer4Tap::create(CCNode* node)
{
	CCEGestureRecognizer4Tap* r = new CCEGestureRecognizer4Tap();
	r->init(node);
	return r;
}

bool CCEGestureRecognizer4Tap::touchBegan(int id, CCPoint touch)
{
	return touchMoved(id, touch);
}

bool CCEGestureRecognizer4Tap::touchMoved(int id, CCPoint touch)
{
	m_tap = false;
	return true;
}

void CCEGestureRecognizer4Tap::touchCancelled(int id, CCPoint touch)
{
	m_tap = false;
}

bool CCEGestureRecognizer4Tap::touchEnded(int id, CCPoint touch)
{
	CCPoint local = m_node->convertToNodeSpace(touch);
	if(m_node->containsPoint(local)) {
		m_tap = true;
		CCETouchEvent e(touch);
		m_node->raiseEvent(NODE_EVENT_TAP, &e);
	}
	return false;
}

bool CCEGestureRecognizer4Tap::isRecognized()
{	
	return m_tap;
}

#include "CCEAppUtil.h"

// Holdpress timer
class Timer4Holdpress : public CCObject
{
public:
	Timer4Holdpress(){};
	virtual ~Timer4Holdpress(){};

public:
	void checkHold(float dt) {
		delegateObj->timerCheckHold(dt);
	}

	CCEGestureRecognizer4Holdpress* delegateObj;
};

//
// CCEGestureRecognizer4Holdpress
//
CCEGestureRecognizer4Holdpress::CCEGestureRecognizer4Holdpress()
	: m_timeThreshold(750)
	, m_moveThreshold(-1)
	, m_checkInterval(0.1f)
	, m_hold(false)
	, m_holdBeginTime(0)
	, m_timer(NULL)
{
	
}

CCEGestureRecognizer4Holdpress::~CCEGestureRecognizer4Holdpress()
{
	clearTimer();
	CC_SAFE_RELEASE_NULL(m_timer);	
}

void CCEGestureRecognizer4Holdpress::clearTimer()
{
	if(isChecking() && m_timer!=NULL) {
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(Timer4Holdpress::checkHold),m_timer);
	}
	m_holdBeginTime = 0;
}

CCEGestureRecognizer4Holdpress* CCEGestureRecognizer4Holdpress::create(CCNode* node)
{
	return create(node, 750, -1, 0.1f);
}

CCEGestureRecognizer4Holdpress* CCEGestureRecognizer4Holdpress::create(CCNode* node,int timeThreshold,int moveThreshold, float checkInterval)
{
	CCEGestureRecognizer4Holdpress* r = new CCEGestureRecognizer4Holdpress();
	r->init(node, timeThreshold, moveThreshold,checkInterval);
	return r;
}

void CCEGestureRecognizer4Holdpress::init(CCNode* node,int timeThreshold,int moveThreshold, float checkInterval)
{
	CCEGestureRecognizer4Node::init(node);
	m_timeThreshold = timeThreshold;
	m_moveThreshold = moveThreshold;
	m_checkInterval = checkInterval;
}

bool CCEGestureRecognizer4Holdpress::touchBegan(int id, CCPoint touch)
{
	CCPoint local = m_node->convertToNodeSpace(touch);
	if(m_node->containsPoint(local)) {
		m_holdPos = touch;
		checkHold(touch);
	}
	return true;
}

bool CCEGestureRecognizer4Holdpress::touchMoved(int id, CCPoint touch)
{
	CCPoint local = m_node->convertToNodeSpace(touch);
	if(m_node->containsPoint(local)) {
		if(!isChecking()) {
			m_holdPos = touch;
		}
		checkHold(touch);
	} else {
		cancelHold(touch);
	}
	return true;
}

void CCEGestureRecognizer4Holdpress::touchCancelled(int id, CCPoint touch)
{
	cancelHold(touch);
}

bool CCEGestureRecognizer4Holdpress::touchEnded(int id, CCPoint touch)
{
	cancelHold(touch);
	return false;
}

bool CCEGestureRecognizer4Holdpress::isRecognized()
{	
	return m_hold;
}

void CCEGestureRecognizer4Holdpress::timerCheckHold(float)
{
	checkHold(m_holdPos);
}

void CCEGestureRecognizer4Holdpress::checkHold(CCPoint pt)
{
	CCPoint local = m_node->convertToNodeSpace(pt);
	if(!m_node->containsPoint(local)) {
		cancelHold(pt);
		return;
	}

	if(m_moveThreshold>0) {
		// check hold position
		if(abs(pt.x-m_holdPos.x)>m_moveThreshold ||
			abs(pt.y-m_holdPos.y)>m_moveThreshold)
		{
			cancelHold(pt);
			return;
		}
	}

	if(m_hold)return;
	
	long tm = CCEAppUtil::timeTick();
	if(!isChecking()) {
		m_holdBeginTime = tm;
		if(m_timer==NULL) {
			Timer4Holdpress* p = new Timer4Holdpress();
			p->delegateObj = this;
			m_timer = p;
		}
		CCDirector::sharedDirector()->
						getScheduler()->scheduleSelector(schedule_selector(Timer4Holdpress::checkHold), m_timer, m_checkInterval,false);
		return;
	}
	
	if(tm - m_holdBeginTime >= m_timeThreshold)
	{
		clearTimer();
		
		m_hold = true;
		CCETouchHoldEvent event(pt, true);
		m_node->raiseEvent(NODE_EVENT_HOLDPRESS, &event);		
	}
}

void CCEGestureRecognizer4Holdpress::cancelHold(CCPoint pt)
{	
	clearTimer();	

	if(m_hold) {
		CCETouchHoldEvent e(pt, false);
		m_node->raiseEvent(NODE_EVENT_HOLDPRESS, &e);
	}
	m_hold = false;
}
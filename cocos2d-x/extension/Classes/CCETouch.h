#ifndef __CCE_TOUCH_H_
#define __CCE_TOUCH_H_

#include "cocos2d.h"
#include <string>
#include <list>

USING_NS_CC;

#define NODE_EVENT_FOCUS		"focus"
#define NODE_EVENT_TAP			"tap"
#define NODE_EVENT_HOLDPRESS	"holdpress"
#define NODE_EVENT_PAN			"pan"
#define NODE_EVENT_PINCH		"pinch"
#define NODE_EVENT_SWIPE		"swipe"
#define NODE_EVENT_ROTATION		"rotation"

typedef struct _CCETouchInfo {
	int id;
	CCPoint touch;
} CCETouchInfo;

class CCETouchEvent : public CCNodeEvent
{
public:
	CCETouchEvent(CCPoint touch){m_touch = touch;};
	virtual ~CCETouchEvent(){};

	CCPoint& getTouch(){return m_touch;};

	virtual CCValue toValue();

protected:
	CCPoint m_touch;
};

class CCETouchFocusEvent : public CCETouchEvent
{
public:
	CCETouchFocusEvent(CCPoint touch, bool focus) : CCETouchEvent(touch) {m_focus = focus;};
	virtual ~CCETouchFocusEvent(){};

	bool isFocus(){return m_focus;};

	virtual CCValue toValue();

protected:
	bool m_focus;
};

class CCETouchHoldEvent : public CCETouchEvent
{
public:
	CCETouchHoldEvent(CCPoint touch, bool hold) : CCETouchEvent(touch) {m_hold = hold;};
	virtual ~CCETouchHoldEvent(){};

	bool isHold(){return m_hold;};

	virtual CCValue toValue();

protected:
	bool m_hold;
};

class CCETouchPanEvent : public CCETouchEvent
{
public:
	CCETouchPanEvent(CCPoint from, CCPoint to) : CCETouchEvent(to) {m_from = from;};
	virtual ~CCETouchPanEvent(){};

	CCPoint getFrom(){return m_from;};

	virtual CCValue toValue();

protected:
	CCPoint m_from;
};

class CCEGestureRecognizer {

public:
	CCEGestureRecognizer();
	virtual ~CCEGestureRecognizer();

public:
	// return stillTrack?
	virtual bool touchBegan(int id, CCPoint touch) = 0;
	virtual bool touchMoved(int id, CCPoint touch) = 0;	
    virtual bool touchEnded(int id, CCPoint touch) = 0;
	virtual void touchCancelled(int id, CCPoint touch) = 0;
	// already recognizer
	virtual bool isRecognized() = 0;
};

class CCEGestureRecognizer4Node : public CCEGestureRecognizer {

public:
	CCEGestureRecognizer4Node();
	virtual ~CCEGestureRecognizer4Node();

public:
	virtual void init(CCNode* node);

	CCNode* getNode(){return m_node;};

protected:
	CCNode* m_node;
};

class CCEGestureRecognizer4Chain : public CCEGestureRecognizer {

public:
	CCEGestureRecognizer4Chain();
	virtual ~CCEGestureRecognizer4Chain();

public:
	CCEGestureRecognizer4Chain* add(CCEGestureRecognizer* sub);
	bool remove(CCEGestureRecognizer* sub,bool deleteObj);
	void clear();

	// return active?
	virtual bool touchBegan(int id, CCPoint touch);
	virtual bool touchMoved(int id, CCPoint touch);
    virtual bool touchEnded(int id, CCPoint touch);
	virtual void touchCancelled(int id, CCPoint touch);
	// already recognize
	virtual bool isRecognized();

protected:
	std::list<CCEGestureRecognizer*> m_chain;
};

class CCEGestureRecognizer4Focus : public CCEGestureRecognizer4Node {

public:
	CCEGestureRecognizer4Focus();
	virtual ~CCEGestureRecognizer4Focus();

public:
	static CCEGestureRecognizer4Focus* create(CCNode* node);

	bool isFocus(){return m_focus;};
	void setFocus(bool v){m_focus = v;};
	void setFocus(CCPoint pt, bool v);

	// return active?
	virtual bool touchBegan(int id, CCPoint touch);
	virtual bool touchMoved(int id, CCPoint touch);	
    virtual bool touchEnded(int id, CCPoint touch);
	virtual void touchCancelled(int id, CCPoint touch);
	// already recognize
	virtual bool isRecognized();

protected:
	bool m_focus;
};

class CCEGestureRecognizer4Tap : public CCEGestureRecognizer4Node {

public:
	CCEGestureRecognizer4Tap();
	virtual ~CCEGestureRecognizer4Tap();

public:
	static CCEGestureRecognizer4Tap* create(CCNode* node);

	// return active?
	virtual bool touchBegan(int id, CCPoint touch);
	virtual bool touchMoved(int id, CCPoint touch);	
    virtual bool touchEnded(int id, CCPoint touch);
	virtual void touchCancelled(int id, CCPoint touch);
	// already recognize
	virtual bool isRecognized();

protected:
	bool m_tap;
};

class CCEGestureRecognizer4Holdpress : public CCEGestureRecognizer4Node {

public:
	CCEGestureRecognizer4Holdpress();
	virtual ~CCEGestureRecognizer4Holdpress();

public:
	virtual void init(CCNode* node, int timeThreshold, int moveThreshold, float checkInterval);

	static CCEGestureRecognizer4Holdpress* create(CCNode* node);
	static CCEGestureRecognizer4Holdpress* create(CCNode* node,int timeThreshold, int moveThreshold, float checkInterval);

	// return active?
	virtual bool touchBegan(int id, CCPoint touch);
	virtual bool touchMoved(int id, CCPoint touch);	
    virtual bool touchEnded(int id, CCPoint touch);
	virtual void touchCancelled(int id, CCPoint touch);
	// already recognize
	virtual bool isRecognized();

	void timerCheckHold(float dt);

protected:
	bool isChecking(){return m_holdBeginTime>0;};
	void checkHold(CCPoint pt);
	void cancelHold(CCPoint pt);
	void clearTimer();

protected:
	float m_checkInterval;
	int m_timeThreshold;
	int m_moveThreshold;

	CCObject* m_timer;
	bool m_hold;
	long m_holdBeginTime;
	CCPoint m_holdPos;
};

#endif

#ifndef __CCE_LAYER_TOUCH_H_
#define __CCE_LAYER_TOUCH_H_

#include "cocos2d.h"
#include <string>
#include <list>

USING_NS_CC;

#define kCCELayerTouchHandlerPriority -16

#define NODE_EVENT_FOCUS		"focus"
#define NODE_EVENT_UNFOCUS		"unfocus"
#define NODE_EVENT_CLICK		"click"
#define NODE_EVENT_HOLD			"hold"
#define NODE_EVENT_UNHOLD		"unhold"

class CCELayerTouchItem {

public:
	CCELayerTouchItem();
	virtual ~CCELayerTouchItem();


public:
	void setPriority(int newPriority){priority=newPriority;}
	int getPriority(){return priority;}

	void setCover(bool v){cover=v;}
	bool isCover(){return cover;}

	void setHoldThreshold(float v){holdThreshold = (long) v*1000;};
	float getHoldThreshold(){return holdThreshold*1.0f/1000;};

	bool operator()(const CCELayerTouchItem* t1,const CCELayerTouchItem* t2);

protected:
	CCNode* node;

	// config
	int priority;
	bool cover;
	long holdThreshold;
		
	// runtime
	bool focus;
	long focusTime;
	bool holded;	

	friend class CCELayerTouch;
};

class CCETouchEvent : public CCNodeEvent
{
public:
	CCETouchEvent(CCPoint touch){m_touch = touch;};
	virtual ~CCETouchEvent(){};

	CCPoint& getTouch(){return m_touch;};

protected:
	CCPoint m_touch;
};

class CCELayerTouch : public CCLayer
{    
public:
    CCELayerTouch();
    virtual ~CCELayerTouch();

    /** creates an empty CCMenu */
    static CCELayerTouch* create();

    /** initializes */
    virtual bool init();

    /** set event handler priority. By default it is: -16 */
    void setHandlerPriority(int newPriority);

    //super methods
    virtual void registerWithTouchDispatcher();

    /**
    @brief For phone event handle functions
    */
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);

	CCELayerTouchItem* createTouch(CCNode* node){return createTouch(node,false);};
	virtual CCELayerTouchItem* createTouch(CCNode* node,bool track);
	CCELayerTouchItem* getTouch(CCNode* node);
	virtual void removeTouch(CCNode* node);
	virtual void removeAllTouch();
	
	
	static CCELayerTouch* getTouchLayer(CCNode* node);

	virtual void onEnter();
    virtual void onExit();
	
	virtual bool isEnabled() { return m_bEnabled; }
    virtual void setEnabled(bool value) { m_bEnabled = value; };

	void setCheckHoldInterval(float v){m_checkHoldInterval = v;};
	float getCheckHoldInterval(){return m_checkHoldInterval;};

	void checkItems(){checkItems(NULL);};	
	void checkHold(float dt);
	void itemTrackHandler(CCNode* node, const char* name, CCNodeEvent*);

protected:
	virtual CCELayerTouchItem* newItem();

	void checkItems(CCPoint* movePoint);	
	void focus(std::list<CCELayerTouchItem*>& items, CCPoint touch);
	void lostFocus(CCELayerTouchItem* item, CCPoint touch);	
	void checkHold();
	
	void getFocusedItems(std::list<CCELayerTouchItem*>& ret);
	void itemsForTouch(std::list<CCELayerTouchItem*>& ret,CCPoint& touchLocation);
	void orderItems(std::list<CCELayerTouchItem*>& ret);    
	
	// config
	bool m_bEnabled;
	float m_checkHoldInterval;

	// runtime
	std::list<CCELayerTouchItem*> m_touchItems;		
	CCPoint m_lastTouch;
	bool m_checkHold;
};

#endif

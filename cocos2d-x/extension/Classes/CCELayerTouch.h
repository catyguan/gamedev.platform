#ifndef __CCE_LAYER_TOUCH_H_
#define __CCE_LAYER_TOUCH_H_

#include "cocos2d.h"
#include <string>
#include <list>

USING_NS_CC;

enum {
    //* priority used by the menu for the event handler
    kCCELayerTouchHandlerPriority = -16,
};

#define NODE_EVENT_FOCUS "focus"
#define NODE_EVENT_UNFOCUS "unfocus"
#define NODE_EVENT_CLICK "click"

class CCELayerTouchItem {

public:
	CCELayerTouchItem();
	~CCELayerTouchItem();


public:
	void setPriority(int newPriority){priority=newPriority;}
	int getPriority(){return priority;}

	void setCover(bool v){cover=v;}
	bool isCover(){return cover;}

	void setTrack(bool v){track=v;}
	bool isTrack(){return track;}

	bool operator()(const CCELayerTouchItem* t1,const CCELayerTouchItem* t2);

protected:
	CCNode* node;

	int priority;
	bool cover;
	bool track;
		
	bool focus;
	bool tracked;

	friend class CCELayerTouch;
};

class CCETouchEvent : public CCNodeEvent
{
public:
	CCETouchEvent(CCPoint touch){m_touch = touch;};
	~CCETouchEvent(){};

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
	CCELayerTouchItem* createTouch(CCNode* node,bool track);
	CCELayerTouchItem* getTouch(CCNode* node);
	void removeTouch(CCNode* node);
	void removeAllTouch();
	
	
	static CCELayerTouch* getTouchLayer(CCNode* node);

    virtual void onExit();
	
	virtual bool isEnabled() { return m_bEnabled; }
    virtual void setEnabled(bool value) { m_bEnabled = value; };

	void checkItems();
	void itemTrackHandler(CCNode* node, const char* name, CCNodeEvent*);

protected:
	void focus(std::list<CCELayerTouchItem*>& items, CCPoint touch);
	void lostFocus(CCELayerTouchItem* item, CCPoint touch);
	void untrack(CCELayerTouchItem* item);
	
    bool itemsForTouch(std::list<CCELayerTouchItem*>& ret,CCPoint& touchPostion, bool track);
	
	std::list<CCELayerTouchItem*> m_touchItems;	
	bool m_bEnabled;

	CCPoint m_lastTouch;
};

#endif

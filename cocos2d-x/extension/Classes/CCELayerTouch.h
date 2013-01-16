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

typedef enum
{
	kTouchTypeNone,
	kTouchTypeFocus,
	kTouchTypeUnFocus,
    kTouchTypeClick,
	kTouchTypeHold,
    kTouchTypeMax
};

typedef void (CCObject::*SEL_TouchHandler)(CCNode* node, int type, CCTouch*);
#define touch_selector(_SELECTOR) (SEL_TouchHandler)(&_SELECTOR)

typedef struct {
	int id;
	int type;	
	CCObject* handleObject;
	SEL_TouchHandler handler;
} CCELayerTouchHandler;

class CCELayerTouchItem {

public:
	CCELayerTouchItem();
	~CCELayerTouchItem();

protected:
	void addEventHandler(int id, int type, CCObject* obj, SEL_TouchHandler handler);

	void call(int checktype, int type, CCTouch* touch);
	void call(int type, CCTouch* touch){call(type,type,touch);}
	
public:
	void setPriority(int newPriority){priority=newPriority;}
	int getPriority(){return priority;}

	void setCover(bool v){cover=v;}
	bool isCover(){return cover;}

	bool hasHandler(int type);

	void onFocus(int id, CCObject* obj, SEL_TouchHandler handler) {
		addEventHandler(id, kTouchTypeFocus, obj, handler);
	}
	void onClick(int id, CCObject* obj, SEL_TouchHandler handler) {
		addEventHandler(id, kTouchTypeClick, obj, handler);
	}
	bool remove(int id);
	void clear();

	bool operator()(const CCELayerTouchItem* t1,const CCELayerTouchItem* t2);

protected:
	CCNode* node;
	std::list<CCELayerTouchHandler*> eventHandlers;
	
	int priority;
	bool cover;
	bool focus;

	friend class CCELayerTouch;
};

class CCELayerTouch : public CCLayer
{    
public:
    CCELayerTouch();
    virtual ~CCELayerTouch();

    /** creates an empty CCMenu */
    static CCELayerTouch* create();

    /** initializes an empty CCMenu */
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

	CCELayerTouchItem* createTouch(CCNode* node);
	CCELayerTouchItem* getTouch(CCNode* node);
	void removeTouch(CCNode* node);
	void removeAllTouch();

    /**
    @since v0.99.5
    override onExit
    */
    virtual void onExit();
	
	virtual bool isEnabled() { return m_bEnabled; }
    virtual void setEnabled(bool value) { m_bEnabled = value; };

protected:
	void focus(std::list<CCELayerTouchItem*>& items, CCTouch* touch);
	void lostFocus(CCELayerTouchItem* item, CCTouch* touch);

    bool itemsForTouch(std::list<CCELayerTouchItem*>& ret,CCTouch * touch);
	
	std::list<CCELayerTouchItem*> m_touchItems;	
	bool m_bEnabled;
};

#endif

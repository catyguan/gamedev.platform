#ifndef __CCE_LAYER_TOUCH_H_
#define __CCE_LAYER_TOUCH_H_

#include "cocos2d.h"
#include "CCETouch.h"
#include <string>
#include <list>

USING_NS_CC;

#define kCCELayerTouchHandlerPriority -16

class CCELayerTouchItem {

public:
	CCELayerTouchItem();
	virtual ~CCELayerTouchItem();


public:
	void setPriority(int newPriority){priority=newPriority;}
	int getPriority(){return priority;}

	bool operator()(const CCELayerTouchItem* t1,const CCELayerTouchItem* t2);

protected:
	// config
	CCEGestureRecognizer* recognizer;
	int priority;

	// runtime
	bool actived;	
		
	friend class CCELayerTouch;
};

class CCELayerTouch : public CCLayer
{    
public:
    CCELayerTouch();
    virtual ~CCELayerTouch();

    /** creates an empty CCELayerTouch */
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

	CCELayerTouchItem* createTouch(CCEGestureRecognizer* recognizer);
	CCELayerTouchItem* getTouch(CCEGestureRecognizer* recognizer);
	CCELayerTouchItem* getTouchByNode(CCNode* node);
	void removeTouch(CCEGestureRecognizer* recognizer);
	void removeTouchByNode(CCNode* node);
	void removeAllTouch();

	void CCELayerTouch::reorderItems();
	
	
	static CCELayerTouch* getTouchLayer(CCNode* node);

	virtual void onEnter();
    virtual void onExit();
	
	virtual bool isEnabled() { return m_bEnabled; }
    virtual void setEnabled(bool value) { m_bEnabled = value; };	

	void checkItems(int* id, CCPoint* pt);	

protected:
	void removeItem(std::list<CCELayerTouchItem*>::const_iterator it);
	void sortItems(std::list<CCELayerTouchItem*>& ret);
	void recognized(int id, CCPoint pt, CCELayerTouchItem* item);
	void addActiveItem(CCELayerTouchItem* item);
	void removeActiveItem(CCELayerTouchItem* item,bool except);
	void _travel(int type,int id, CCPoint pt);
	void travel(int type, int id, CCPoint pt);

	// config
	bool m_bEnabled;	

	// runtime
	std::list<CCELayerTouchItem*> m_touchItems;
	std::list<CCELayerTouchItem*> m_activedItems;
	int m_lastId;
	CCPoint m_lastTouch;
	bool m_traveling;
	std::list<CCELayerTouchItem*> m_removedItems;

	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(enableTouch)
	CC_DECLARE_CALL(disableTouch)
	CC_DECLARE_CALLS_END	

};

class CCETouchBuilder
{
public:
	CCETouchBuilder();
	virtual ~CCETouchBuilder();

public:
	CCETouchBuilder& bind(CCNode* node);
	CCETouchBuilder& on(std::string type);
	CCETouchBuilder& onFocus(CCObject* obj,SEL_NodeEventHandler handler);
	CCETouchBuilder& onTap(CCObject* obj,SEL_NodeEventHandler handler);
	CCETouchBuilder& onHoldpress(CCObject* obj,SEL_NodeEventHandler handler);
	CCETouchBuilder& onHoldpress(CCObject* obj,SEL_NodeEventHandler handler,int timeThreshold, int moveThreshold, float checkInterval);

	void setPriority(int newPriority){priority=newPriority;}
	int getPriority(){return priority;}

public:
	std::list<CCEGestureRecognizer*>& getRecognizers(){return recognizers;};
	void clear(bool del);
	
	CCETouchBuilder& addGestureRecognizer(CCEGestureRecognizer* obj);
	void createTouch(CCELayerTouch* layer);

protected:
	CCNode* node;
	int priority;

	std::list<CCEGestureRecognizer*> recognizers;
};

#endif

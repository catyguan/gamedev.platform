#ifndef __CCE_SCROLLVIEW_H__
#define __CCE_SCROLLVIEW_H__

#include "cocos2d.h"

USING_NS_CC;

typedef enum {
	kCCEScrollViewDirectionNone = -1,
    kCCEScrollViewDirectionHorizontal = 0,
    kCCEScrollViewDirectionVertical,
    kCCEScrollViewDirectionBoth
} CCEScrollViewDirection;

class CCEScrollViewBase;

#define NODE_EVENT_SCROLLVIEW_SCROLL	"scrollview.scroll"
#define NODE_EVENT_SCROLLVIEW_ZOOM		"scrollview.zoom"

/**
 * ScrollView support for cocos2d for iphone.
 * It provides scroll view functionalities to cocos2d projects natively.
 */
class CCEScrollViewBase : public CCLayer
{
public:
    CCEScrollViewBase();
    virtual ~CCEScrollViewBase();

    bool init();

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static CCEScrollViewBase* create(CCSize size, CCNode* container = NULL);

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static CCEScrollViewBase* create();

    /**
     * Returns a scroll view object
     *
     * @param size view size
     * @param container parent object
     * @return scroll view object
     */
    virtual bool initWithViewSize(CCSize size, CCNode* container = NULL);


    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     *
     * @param offset new offset
     * @param If YES, the view scrolls to the new offset
     */
    void setContentOffset(CCPoint offset, bool animated = false);
    CCPoint getContentOffset();

	CCPoint getScrollOffset(CCPoint& moveDistance);
	void scroll(CCPoint moveDistance, bool animated = false);
	void scrollInDuration(CCPoint moveDistance, float dt); 

    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     * You can override the animation duration with this method.
     *
     * @param offset new offset
     * @param animation duration
     */
    void setContentOffsetInDuration(CCPoint offset, float dt); 

    void setZoomScale(float s);
    /**
     * Sets a new scale and does that for a predefined duration.
     *
     * @param s a new scale vale
     * @param animated if YES, scaling is animated
     */
    void setZoomScale(float s, bool animated);

    float getZoomScale();

    /**
     * Sets a new scale for container in a given duration.
     *
     * @param s a new scale value
     * @param animation duration
     */
    void setZoomScaleInDuration(float s, float dt);

    /**
     * Returns the current container's minimum offset. You may want this while you animate scrolling by yourself
     */
    CCPoint minContainerOffset();
    /**
     * Returns the current container's maximum offset. You may want this while you animate scrolling by yourself
     */
    CCPoint maxContainerOffset();

    /**
     * Determines if a given node's bounding box is in visible bounds
     *
     * @return YES if it is in visible bounds
     */
    bool isNodeVisible(CCNode * node);

    /**
     * Provided to make scroll view compatible with SWLayer's pause method
     */
    void pause(CCObject* sender);

    /**
     * Provided to make scroll view compatible with SWLayer's resume method
     */
    void resume(CCObject* sender);

	bool isDragging() {return m_bDragging;}	
    bool isBounceable() { return m_bBounceable; }
    void setBounceable(bool bBounceable) { m_bBounceable = bBounceable; }

	bool isScrollOut() { return m_bScrollOut; }
    void setScrollOut(bool v) { m_bScrollOut = v; }

    /**
     * size to clip. CCNode boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    CCSize getViewSize() { return m_tViewSize; } 
    void setViewSize(CCSize size);

    CCNode * getContainer();
    void setContainer(CCNode * pContainer);

    /**
     * direction allowed to scroll. CCEScrollViewDirectionBoth by default.
     */
    CCEScrollViewDirection getDirection() { return m_eDirection; }
    virtual void setDirection(CCEScrollViewDirection eDirection) { m_eDirection = eDirection; }

    virtual void setContentSize(const CCSize & size);
    virtual const CCSize& getContentSize();

	void updateInset();
    /**
     * Determines whether it clips its children or not.
     */
    bool isClippingToBounds() { return m_bClippingToBounds; }
    void setClippingToBounds(bool bClippingToBounds) { m_bClippingToBounds = bClippingToBounds; }

	virtual void visit();
    virtual void addChild(CCNode * child, int zOrder, int tag);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode * child);

protected:
	virtual CCPoint getCenterPoint();

	void setDragging(bool v){m_bDragging = v;};

    /**
     * Init this object with a given size to clip its content.
     *
     * @param size view size
     * @return initialized scroll view object
     */
    bool initWithViewSize(CCSize size);
    /**
     * Relocates the container at the proper offset, in bounds of max/min offsets.
     *
     * @param animated If YES, relocation is animated
     */
	bool getRelocatePosition(CCPoint& pt);
    void relocateContainer(bool animated);

    /**
     * implements auto-scrolling behavior. change SCROLL_DEACCEL_RATE as needed to choose
     * deacceleration speed. it must be less than 1.0f.
     *
     * @param dt delta
     */
    void deaccelerateScrolling(float dt);
    /**
     * This method makes sure auto scrolling causes delegate to invoke its method
     */
    void performedAnimatedScroll(float dt);
    /**
     * Expire animated scroll delegate calls
     */
    void stoppedAnimatedScroll(CCNode* node);
    /**
     * clip this view so that outside of the visible bounds can be hidden.
     */
    void beforeDraw();
    /**
     * retract what's done in beforeDraw so that there's no side effect to
     * other nodes.
     */
    void afterDraw();
    /**
     * Zoom handling
     */
    void handleZoom();

protected:
    /**
     * current zoom scale
     */
    float m_fZoomScale;
    /**
     * min zoom scale
     */
    float m_fMinZoomScale;
    /**
     * max zoom scale
     */
    float m_fMaxZoomScale;

    CCEScrollViewDirection m_eDirection;
    /**
     * If YES, the view is being dragged.
     */
    bool m_bDragging;

    /**
     * Content offset. Note that left-bottom point is the origin
     */
    CCPoint m_tContentOffset;

    /**
     * Container holds scroll view contents, Sets the scrollable container object of the scroll view
     */
    CCNode* m_pContainer;    
    
    /**
     * Determines whether the scroll view is allowed to bounce or not.
     */
    bool m_bBounceable;

    bool m_bClippingToBounds;

    /**
     * scroll speed
     */
    CCPoint m_tScrollDistance;    
    /**
     * size to clip. CCNode boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    CCSize m_tViewSize;
    /**
     * max and min scale
     */
    float m_fMinScale, m_fMaxScale;

	/**
     * max inset point to limit scrolling by touch
     */
    CCPoint m_fMaxInset;
    /**
     * min inset point to limit scrolling by touch
     */
    CCPoint m_fMinInset; 

	bool m_bScrollOut;

	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(viewSize)
	CC_DECLARE_CALL(scrollBy)
	CC_DECLARE_CALL(scrollTo)
	CC_DECLARE_CALL(contentOffset)
	CC_DECLARE_CALL(nodeVisible)
	CC_DECLARE_CALL(direction)
	CC_DECLARE_CALL(container)
	CC_DECLARE_CALLS_END
};

class CCELayer4ScrollVMode : public CCLayer
{
public:
	CCELayer4ScrollVMode();
    virtual ~CCELayer4ScrollVMode();

    static CCELayer4ScrollVMode* create(CCSize contentSize, CCSize tileSize);

public:
	virtual void onEnter();
	virtual void setPosition(const CCPoint &position);

	virtual void vmodeHelper(bool remove, int x, int y);

protected:
	void updateContent();
	CCSize getViewSize();
	bool empty(CCRect& r);
	bool containsPoint(CCRect& r, float x, float y);

protected:
	CCRect m_lastRect;
	CCSize m_tileSize;
};

#include "CCETouch.h"

/**
 * ScrollView support for cocos2d for iphone.
 * It provides scroll view functionalities to cocos2d projects natively.
 */
class CCEScrollView : public CCEScrollViewBase
{
public:
    CCEScrollView();
    virtual ~CCEScrollView();

    virtual void registerWithTouchDispatcher();

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static CCEScrollView* create(CCSize size, CCNode* container = NULL);

    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static CCEScrollView* create();

	virtual bool initWithViewSize(CCSize size, CCNode* container = NULL);

    bool isTouchMoved() { return m_bTouchMoved; }
	bool isActived() {return m_bTouchMoved || m_bDragging;};

    /** override functions */
    // optional
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

	virtual bool touchBegan(int id, CCPoint touch);
	virtual bool touchMoved(int id, CCPoint touch);	
    virtual bool touchEnded(int id, CCPoint touch);
	virtual void touchCancelled(int id, CCPoint touch);

    void setTouchEnabled(bool e);
protected:
	virtual CCPoint getCenterPoint();
	bool hasTouch(int id);
	void removeTouch(int id);

protected:
    /**
     * Determiens whether user touch is moved after begin phase.
     */
    bool m_bTouchMoved;
       
    /**
     * Touch point
     */
    CCPoint m_tTouchPoint;
    /**
     * length between two fingers
     */
    float m_fTouchLength;
    /**
     * UITouch objects to detect multitouch
     */
	std::vector<CCETouchInfo> m_Touches;

	bool m_animated;

	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(enableTouch)
	CC_DECLARE_CALL(animated)
	CC_DECLARE_CALLS_END
};

#endif /* __CCEScrollView_H__ */

#include "CCEScrollView.h"
#include "actions/CCActionInterval.h"
#include "actions/CCActionTween.h"
#include "actions/CCActionInstant.h"
#include "support/CCPointExtension.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "effects/CCGrid.h"
#include "CCDirector.h"
#include "kazmath/GL/matrix.h"
#include "CCEGLView.h"

USING_NS_CC;

#define SCROLL_DEACCEL_RATE  0.95f
#define SCROLL_DEACCEL_DIST  1.0f
#define BOUNCE_DURATION      0.15f
#define INSET_RATIO          0.2f


CCEScrollViewBase::CCEScrollViewBase()
: m_fZoomScale(0.0f)
, m_fMinZoomScale(0.0f)
, m_fMaxZoomScale(0.0f)
, m_bDragging(false)
, m_bBounceable(false)
, m_eDirection(kCCEScrollViewDirectionBoth)
, m_bClippingToBounds(false)
, m_pContainer(NULL)
, m_fMinScale(0.0f)
, m_fMaxScale(0.0f)
,m_bScrollOut(true)
{

}

CCEScrollViewBase::~CCEScrollViewBase()
{
    
}

CCEScrollViewBase* CCEScrollViewBase::create(CCSize size, CCNode* container/* = NULL*/)
{
    CCEScrollViewBase* pRet = new CCEScrollViewBase();
    if (pRet && pRet->initWithViewSize(size, container))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

CCEScrollViewBase* CCEScrollViewBase::create()
{
    CCEScrollViewBase* pRet = new CCEScrollViewBase();
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


bool CCEScrollViewBase::initWithViewSize(CCSize size, CCNode *container/* = NULL*/)
{
    if (CCLayer::init())
    {
        m_pContainer = container;
        
        if (!this->m_pContainer)
        {
            m_pContainer = CCLayer::create();
            this->m_pContainer->ignoreAnchorPointForPosition(false);
            this->m_pContainer->setAnchorPoint(ccp(0.0f, 0.0f));
        }

        this->setViewSize(size);

        m_bBounceable = true;
        m_bClippingToBounds = true;
        m_eDirection  = kCCEScrollViewDirectionBoth;
        m_pContainer->setPosition(ccp(0.0f, 0.0f));
        
        this->addChild(m_pContainer);
        m_fMinScale = m_fMaxScale = 1.0f;
        return true;
    }
    return false;
}

bool CCEScrollViewBase::init()
{
    return this->initWithViewSize(CCSizeMake(200, 200), NULL);
}

bool CCEScrollViewBase::isNodeVisible(CCNode* node)
{
    const CCPoint offset = this->getContentOffset();
    const CCSize  size   = this->getViewSize();
    const float   scale  = this->getZoomScale();
    
    CCRect viewRect;
    
    viewRect = CCRectMake(-offset.x/scale, -offset.y/scale, size.width/scale, size.height/scale); 
    
    return viewRect.intersectsRect(node->boundingBox());
}

void CCEScrollViewBase::pause(CCObject* sender)
{
    m_pContainer->pauseSchedulerAndActions();

    CCObject* pObj = NULL;
    CCArray* pChildren = m_pContainer->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->pauseSchedulerAndActions();
    }
}

void CCEScrollViewBase::resume(CCObject* sender)
{
    CCObject* pObj = NULL;
    CCArray* pChildren = m_pContainer->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->resumeSchedulerAndActions();
    }

    m_pContainer->resumeSchedulerAndActions();
}

void CCEScrollViewBase::setContentOffset(CCPoint offset, bool animated/* = false*/)
{
    if (animated)
    { //animate scrolling
        this->setContentOffsetInDuration(offset, BOUNCE_DURATION);
    } 
    else
    { //set the container position directly
        if (!m_bBounceable)
        {
            const CCPoint minOffset = this->minContainerOffset();
            const CCPoint maxOffset = this->maxContainerOffset();
            
            offset.x = MAX(minOffset.x, MIN(maxOffset.x, offset.x));
            offset.y = MAX(minOffset.y, MIN(maxOffset.y, offset.y));
        }

        m_pContainer->setPosition(offset);

		CCNodeEvent e;
		raiseEvent(NODE_EVENT_SCROLLVIEW_SCROLL, &e);
    }
}

void CCEScrollViewBase::setContentOffsetInDuration(CCPoint offset, float dt)
{
    CCFiniteTimeAction *scroll, *expire;
    
    scroll = CCMoveTo::create(dt, offset);
    expire = CCCallFuncN::create(this, callfuncN_selector(CCEScrollViewBase::stoppedAnimatedScroll));
    m_pContainer->runAction(CCSequence::create(scroll, expire, NULL));
    this->schedule(schedule_selector(CCEScrollViewBase::performedAnimatedScroll));
}

CCPoint CCEScrollViewBase::getContentOffset()
{
    return m_pContainer->getPosition();
}

CCPoint CCEScrollViewBase::getScrollOffset(CCPoint& moveDistance)
{
	switch (m_eDirection)
    {
		case kCCEScrollViewDirectionVertical:
			moveDistance = ccp(0.0f, moveDistance.y);
            break;
		case kCCEScrollViewDirectionHorizontal:
			moveDistance = ccp(moveDistance.x, 0.0f);
            break;
		default:
			break;
	}
                
    CCPoint pt = ccp(m_pContainer->getPosition().x + moveDistance.x, m_pContainer->getPosition().y + moveDistance.y);
	if(!m_bScrollOut) {
		getRelocatePosition(pt);
	}
	return pt;
}

void CCEScrollViewBase::scroll(CCPoint moveDistance, bool animated) 
{
	CCPoint offset = getScrollOffset(moveDistance);
    m_tScrollDistance = moveDistance;
    this->setContentOffset(offset, animated);    
}

void CCEScrollViewBase::scrollInDuration(CCPoint moveDistance, float dt) 
{
	CCPoint offset = getScrollOffset(moveDistance);
    m_tScrollDistance = moveDistance;
    this->setContentOffsetInDuration(offset, dt);    
}

CCPoint CCEScrollViewBase::getCenterPoint()
{
	CCPoint center = ccp(m_tViewSize.width*0.5f, m_tViewSize.height*0.5f);
	return this->convertToWorldSpace(center);
}

void CCEScrollViewBase::setZoomScale(float s)
{
    if (m_pContainer->getScale() != s)
    {
        CCPoint oldCenter, newCenter;
		CCPoint center = getCenterPoint();        
        
        oldCenter = m_pContainer->convertToNodeSpace(center);
        m_pContainer->setScale(MAX(m_fMinScale, MIN(m_fMaxScale, s)));
        newCenter = m_pContainer->convertToWorldSpace(oldCenter);
        
        const CCPoint offset = ccpSub(center, newCenter);
        this->setContentOffset(ccpAdd(m_pContainer->getPosition(),offset));

		CCNodeEvent e;
		raiseEvent(NODE_EVENT_SCROLLVIEW_ZOOM, &e);
    }
}

float CCEScrollViewBase::getZoomScale()
{
    return m_pContainer->getScale();
}

void CCEScrollViewBase::setZoomScale(float s, bool animated)
{
    if (animated)
    {
        this->setZoomScaleInDuration(s, BOUNCE_DURATION);
    }
    else
    {
        this->setZoomScale(s);
    }
}

void CCEScrollViewBase::setZoomScaleInDuration(float s, float dt)
{
    if (dt > 0)
    {
        if (m_pContainer->getScale() != s)
        {
            CCActionTween *scaleAction;
            scaleAction = CCActionTween::create(dt, "zoomScale", m_pContainer->getScale(), s);
            this->runAction(scaleAction);
        }
    }
    else
    {
        this->setZoomScale(s);
    }
}

void CCEScrollViewBase::setViewSize(CCSize size)
{
    m_tViewSize = size;
    CCLayer::setContentSize(size);
}

CCNode * CCEScrollViewBase::getContainer()
{
    return this->m_pContainer;
}

void CCEScrollViewBase::setContainer(CCNode * pContainer)
{
    this->removeAllChildrenWithCleanup(true);

    if (!pContainer) return;

    this->m_pContainer = pContainer;

    this->m_pContainer->ignoreAnchorPointForPosition(false);
    this->m_pContainer->setAnchorPoint(ccp(0.0f, 0.0f));

    this->addChild(this->m_pContainer);

    this->setViewSize(this->m_tViewSize);
}

bool CCEScrollViewBase::getRelocatePosition(CCPoint& pt)
{
	CCPoint oldPoint, min, max;
    float newX, newY;
    
    min = this->minContainerOffset();
    max = this->maxContainerOffset();

	oldPoint = pt;
    newX     = oldPoint.x;
    newY     = oldPoint.y;
    if (m_eDirection == kCCEScrollViewDirectionBoth || m_eDirection == kCCEScrollViewDirectionHorizontal)
    {
        newX     = MAX(newX, min.x);
        newX     = MIN(newX, max.x);
    }

    if (m_eDirection == kCCEScrollViewDirectionBoth || m_eDirection == kCCEScrollViewDirectionVertical)
    {
        newY     = MIN(newY, max.y);
        newY     = MAX(newY, min.y);
    }

	pt.y = newY;
	pt.x = newX;

    return (newY != oldPoint.y || newX != oldPoint.x);
}

void CCEScrollViewBase::relocateContainer(bool animated)
{
	CCPoint pt = m_pContainer->getPosition();
	if (getRelocatePosition(pt))
    {
        this->setContentOffset(pt, animated);
    }
}

CCPoint CCEScrollViewBase::maxContainerOffset()
{
    return ccp(0.0f, 0.0f);
}

CCPoint CCEScrollViewBase::minContainerOffset()
{
    return ccp(m_tViewSize.width - m_pContainer->getContentSize().width*m_pContainer->getScaleX(), 
               m_tViewSize.height - m_pContainer->getContentSize().height*m_pContainer->getScaleY());
}

void CCEScrollViewBase::deaccelerateScrolling(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(CCEScrollViewBase::deaccelerateScrolling));
        return;
    }
    
    float newX, newY;
    CCPoint maxInset, minInset;
    
	CCPoint pt = ccpAdd(m_pContainer->getPosition(), m_tScrollDistance);
	if(!m_bScrollOut) {
		getRelocatePosition(pt);
	}
    m_pContainer->setPosition(pt);
    
    if (m_bBounceable)
    {
        maxInset = m_fMaxInset;
        minInset = m_fMinInset;
    }
    else
    {
        maxInset = this->maxContainerOffset();
        minInset = this->minContainerOffset();
    }
    
    //check to see if offset lies within the inset bounds
    newX     = MIN(m_pContainer->getPosition().x, maxInset.x);
    newX     = MAX(newX, minInset.x);
    newY     = MIN(m_pContainer->getPosition().y, maxInset.y);
    newY     = MAX(newY, minInset.y);
    
    newX = m_pContainer->getPosition().x;
    newY = m_pContainer->getPosition().y;
    
    m_tScrollDistance     = ccpSub(m_tScrollDistance, ccp(newX - m_pContainer->getPosition().x, newY - m_pContainer->getPosition().y));
    m_tScrollDistance     = ccpMult(m_tScrollDistance, SCROLL_DEACCEL_RATE);
    this->setContentOffset(ccp(newX,newY));
    
    if ((fabsf(m_tScrollDistance.x) <= SCROLL_DEACCEL_DIST &&
         fabsf(m_tScrollDistance.y) <= SCROLL_DEACCEL_DIST) ||
        newY > maxInset.y || newY < minInset.y ||
        newX > maxInset.x || newX < minInset.x ||
        newX == maxInset.x || newX == minInset.x ||
        newY == maxInset.y || newY == minInset.y)
    {
        this->unschedule(schedule_selector(CCEScrollViewBase::deaccelerateScrolling));
        this->relocateContainer(true);
    }
}

void CCEScrollViewBase::stoppedAnimatedScroll(CCNode * node)
{
    this->unschedule(schedule_selector(CCEScrollViewBase::performedAnimatedScroll));
}

void CCEScrollViewBase::performedAnimatedScroll(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(CCEScrollViewBase::performedAnimatedScroll));
        return;
    }

	CCNodeEvent e;
	raiseEvent(NODE_EVENT_SCROLLVIEW_SCROLL, &e);
}


CCSize CCEScrollViewBase::getContentSize()
{
	return m_pContainer->getContentSize();
}

void CCEScrollViewBase::setContentSize(const CCSize & size)
{
    if (this->getContainer() != NULL)
    {
        this->getContainer()->setContentSize(size);
		this->updateInset();
    }
}

void CCEScrollViewBase::updateInset()
{
	if (this->getContainer() != NULL)
	{
		m_fMaxInset = this->maxContainerOffset();
		m_fMaxInset = ccp(m_fMaxInset.x + m_tViewSize.width * INSET_RATIO,
			m_fMaxInset.y + m_tViewSize.height * INSET_RATIO);
		m_fMinInset = this->minContainerOffset();
		m_fMinInset = ccp(m_fMinInset.x - m_tViewSize.width * INSET_RATIO,
			m_fMinInset.y - m_tViewSize.height * INSET_RATIO);
	}
}

/**
 * make sure all children go to the container
 */
void CCEScrollViewBase::addChild(CCNode * child, int zOrder, int tag)
{
    child->ignoreAnchorPointForPosition(false);
    child->setAnchorPoint(ccp(0.0f, 0.0f));
    if (m_pContainer != child) {
        m_pContainer->addChild(child, zOrder, tag);
    } else {
        CCLayer::addChild(child, zOrder, tag);
    }
}

void CCEScrollViewBase::addChild(CCNode * child, int zOrder)
{
    this->addChild(child, zOrder, child->getTag());
}

void CCEScrollViewBase::addChild(CCNode * child)
{
    this->addChild(child, child->getZOrder(), child->getTag());
}

/**
 * clip this view so that outside of the visible bounds can be hidden.
 */
void CCEScrollViewBase::beforeDraw()
{
    if (m_bClippingToBounds)
    {
		// TODO: This scrollview should respect parents' positions
		CCPoint screenPos = this->getParent()->convertToWorldSpace(this->getPosition());

        glEnable(GL_SCISSOR_TEST);
        float s = this->getScale();

//        CCDirector *director = CCDirector::sharedDirector();
//        s *= director->getContentScaleFactor();
        CCEGLView::sharedOpenGLView()->setScissorInPoints(screenPos.x*s, screenPos.y*s, m_tViewSize.width*s, m_tViewSize.height*s);
        //glScissor((GLint)screenPos.x, (GLint)screenPos.y, (GLsizei)(m_tViewSize.width*s), (GLsizei)(m_tViewSize.height*s));
		
    }
}

/**
 * retract what's done in beforeDraw so that there's no side effect to
 * other nodes.
 */
void CCEScrollViewBase::afterDraw()
{
    if (m_bClippingToBounds)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void CCEScrollViewBase::visit()
{
	// quick return if not visible
	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();
    this->beforeDraw();

	if(m_pChildren)
    {
		ccArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode *child =  (CCNode*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode* child = (CCNode*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }

    this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }

	kmGLPopMatrix();
}

#include "touch_dispatcher/CCTouch.h"

CCEScrollView::CCEScrollView()
: m_bTouchMoved(false)
, m_fTouchLength(0.0f)
, m_pTouches(NULL)
{

}

CCEScrollView::~CCEScrollView()
{
    m_pTouches->release();
}

CCEScrollView* CCEScrollView::create(CCSize size, CCNode* container/* = NULL*/)
{
    CCEScrollView* pRet = new CCEScrollView();
    if (pRet && pRet->initWithViewSize(size, container))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

CCEScrollView* CCEScrollView::create()
{
    CCEScrollView* pRet = new CCEScrollView();
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


bool CCEScrollView::initWithViewSize(CCSize size, CCNode *container/* = NULL*/)
{
	if (CCEScrollViewBase::initWithViewSize(size, container))
    {
        setTouchEnabled(true);
        m_pTouches = new CCArray();
        m_fTouchLength = 0.0f;
        return true;
    }
    return false;
}

void CCEScrollView::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
}

void CCEScrollView::setTouchEnabled(bool e)
{
    CCLayer::setTouchEnabled(e);
    if (!e)
    {
        m_bDragging = false;
        m_bTouchMoved = false;
        m_pTouches->removeAllObjects();
    }
}

bool CCEScrollView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return false;
    }
    CCRect frame;
    CCPoint frameOriginal = this->getParent()->convertToWorldSpace(this->getPosition());
    frame = CCRectMake(frameOriginal.x, frameOriginal.y, m_tViewSize.width, m_tViewSize.height);
    
    //dispatcher does not know about clipping. reject touches outside visible bounds.
    if (m_pTouches->count() > 2 ||
        m_bTouchMoved          ||
        !frame.containsPoint(m_pContainer->convertToWorldSpace(m_pContainer->convertTouchToNodeSpace(touch))))
    {
        return false;
    }

    if (!m_pTouches->containsObject(touch))
    {
        m_pTouches->addObject(touch);
    }

    if (m_pTouches->count() == 1)
    { // scrolling
        m_tTouchPoint     = this->convertTouchToNodeSpace(touch);
        m_bTouchMoved     = false;
        m_bDragging     = true; //dragging started
        m_tScrollDistance = ccp(0.0f, 0.0f);
        m_fTouchLength    = 0.0f;
    }
    else if (m_pTouches->count() == 2)
    {
        m_tTouchPoint  = ccpMidpoint(this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                   this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
        m_fTouchLength = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                   m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
        m_bDragging  = false;
    } 
    return true;
}

void CCEScrollView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }

    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bDragging)
        { // scrolling
            CCPoint moveDistance, newPoint;
            CCRect  frame;
            
            m_bTouchMoved  = true;
            CCPoint frameOriginal = this->getParent()->convertToWorldSpace(this->getPosition());
            frame = CCRectMake(frameOriginal.x, frameOriginal.y, m_tViewSize.width, m_tViewSize.height);

            newPoint     = this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0));
            moveDistance = ccpSub(newPoint, m_tTouchPoint);
            m_tTouchPoint  = newPoint;
            
            if (frame.containsPoint(this->convertToWorldSpace(newPoint)))
            {
				scroll(moveDistance);
            }
        }
        else if (m_pTouches->count() == 2 && !m_bDragging)
        {
            const float len = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                            m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            this->setZoomScale(this->getZoomScale()*len/m_fTouchLength);
        }
    }
}

void CCEScrollView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }
    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bTouchMoved)
        {
            this->schedule(schedule_selector(CCEScrollView::deaccelerateScrolling));
        }
        m_pTouches->removeObject(touch);
    } 

    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

void CCEScrollView::ccTouchCancelled(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }
    m_pTouches->removeObject(touch); 
    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

CCPoint CCEScrollView::getCenterPoint()
{
	 if (m_fTouchLength == 0.0f) 
	{
		return CCEScrollView::getCenterPoint();
	}
    else
    {
		return m_tTouchPoint;
	}
}
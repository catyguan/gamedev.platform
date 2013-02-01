#include "CCELayerTouch.h"
#include "CCDirector.h"
#include "CCApplication.h"
#include "support/CCPointExtension.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "touch_dispatcher/CCTouch.h"
#include "base_nodes/CCNode_Events.h"
#include "CCEAppUtil.h"

//
//CCELayerTouchItem
//
CCELayerTouchItem::CCELayerTouchItem()
	: node(NULL)
	, priority(0)
	, cover(true)
	, holdThreshold(750)
	, focus(false)
	, focusTime(0)
	, holded(false)
{
}

CCELayerTouchItem::~CCELayerTouchItem()
{
	CC_SAFE_RELEASE(node);	
}

bool CCELayerTouchItem::operator() (const CCELayerTouchItem* t1,const CCELayerTouchItem* t2)
{
	if(t1->priority > t2->priority) return true;
	if(t1->node->getZOrder() > t2->node->getZOrder())return true;
	return false;
}

//
//CCELayerTouch
//
CCELayerTouch::CCELayerTouch()
	: m_bEnabled(true)
	, m_checkHoldInterval(0.1f)
	, m_checkHold(false)
{	
	
}

CCELayerTouch::~CCELayerTouch()
{
	removeAllTouch();
}

CCELayerTouch* CCELayerTouch::create()
{    
    CCELayerTouch *pRet = new CCELayerTouch();
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

bool CCELayerTouch::init()
{
    if (!CCLayer::init())
    {
		return false;
	}
	
    setTouchPriority(kCCELayerTouchHandlerPriority);
    setTouchMode(kCCTouchesOneByOne);
    setTouchEnabled(true);

	return true;
}

void CCELayerTouch::onEnter()
{
	CCLayer::onEnter();
	m_checkHold = false;
}

void CCELayerTouch::onExit()
{
	removeAllTouch();
    CCLayer::onExit();
}

CCELayerTouchItem* CCELayerTouch::newItem()
{
	return new CCELayerTouchItem();
}

CCELayerTouchItem* CCELayerTouch::createTouch(CCNode* node,bool track)
{
	CC_ASSERT(node);

	CCELayerTouchItem* item = getTouch(node);
	if(item!=NULL)return item;
	item = newItem();
	item->node = node;
	node->retain();
	m_touchItems.push_back(item);
	return item;
}

CCELayerTouchItem* CCELayerTouch::getTouch(CCNode* node)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
    {
        CCELayerTouchItem* pItem = (*it);
		if(pItem->node==node)return pItem;
    }
	return NULL;
}

void CCELayerTouch::removeTouch(CCNode* node)
{
	std::list<CCELayerTouchItem*>::const_iterator it,cur;
	for(it=m_touchItems.begin();it!=m_touchItems.end();)
    {
		cur = it;
		it++;
        CCELayerTouchItem* pItem = (*cur);
		if(pItem->node==node) {
			m_touchItems.erase(cur);
			delete pItem;
		}
    }
}

void CCELayerTouch::removeAllTouch()
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
    {
        CCELayerTouchItem* pItem = (*it);
		delete pItem;
    }
	m_touchItems.clear();
}

//Menu - Events
void CCELayerTouch::setHandlerPriority(int newPriority)
{
    CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    pDispatcher->setPriority(newPriority, this);
}

void CCELayerTouch::registerWithTouchDispatcher()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCELayerTouchHandlerPriority, true);
}

void CCELayerTouch::focus(std::list<CCELayerTouchItem*>& items, CCPoint touch)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=items.begin();it!=items.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(!item->focus) {
			item->focus = true;
			CCETouchEvent event(touch);
			item->node->raiseEvent(NODE_EVENT_FOCUS,&event);

			if(item->node->hasEventHandler(NODE_EVENT_HOLD)) {
				if(item->focusTime==0)item->focusTime = CCEAppUtil::timeTick();
				if(!m_checkHold) {
					m_checkHold = true;				
					CCDirector::sharedDirector()->
						getScheduler()->scheduleSelector(schedule_selector(CCELayerTouch::checkHold), this, m_checkHoldInterval,false);
				}
			}
		}
	}
}

bool CCELayerTouch::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
	CCLOG("ccTouchBegan - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);
	
    // if (m_eState != kCCELayerTouchStateWaiting || ! m_bVisible || !m_bEnabled)
	if (!m_bVisible || !m_bEnabled)
    {
        return false;
    }
    for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
    {
        if (c->isVisible() == false)
        {
            return false;
        }
    }

	CCPoint cp = touch->getLocation();
	std::list<CCELayerTouchItem*> items;
	itemsForTouch(items, cp);
	if (items.size()>0)
	{
		focus(items,cp);
		m_lastTouch = cp;
		return true;
	}
	m_lastTouch = cp;	

	CCRect r = boundingBox();
	r.origin = CCPointZero;
	if(r.isEqual(&CCRectZero)) {
		return true;
	}
	CCPoint local = getParent()->convertToNodeSpace(m_lastTouch);
	return r.containsPoint(local);
}

void CCELayerTouch::lostFocus(CCELayerTouchItem* item, CCPoint touch)
{
	CCETouchEvent event(touch);
	if(item->holded) {
		item->holded = false;
		item->node->raiseEvent(NODE_EVENT_UNHOLD,&event);	
	}

	item->focus = false;
	item->focusTime = 0;	
	item->node->raiseEvent(NODE_EVENT_UNFOCUS,&event);	
}

void CCELayerTouch::ccTouchEnded(CCTouch *touch, CCEvent* event)
{    
    CC_UNUSED_PARAM(event);
	CCLOG("ccTouchEnded - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(item->focus) {
			CCETouchEvent event(touch->getLocation());
			item->node->raiseEvent(NODE_EVENT_CLICK, &event);
			lostFocus(item, touch->getLocation());
		}
	}
}

void CCELayerTouch::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{    
    CC_UNUSED_PARAM(event);
	CCLOG("ccTouchCancelled - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(item->focus) {
			lostFocus(item, touch->getLocation());
		}
	}
}

void CCELayerTouch::checkItems(CCPoint* movePoint)
{
	CCPoint cp = movePoint!=NULL?*movePoint:m_lastTouch;

	std::list<CCELayerTouchItem*> items;
	itemsForTouch(items, cp);
    if(items.size()>0) {
		focus(items,cp);
	}
	
    std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(item->focus) {
			bool f = false;
			std::list<CCELayerTouchItem*>::const_iterator it2;
			for(it2=items.begin();it2!=items.end();it2++) {
				CCELayerTouchItem* item2 = (*it2);
				if(item==item2) {
					f = true;
					break;
				}
			}
			if(!f) {				
				lostFocus(item,cp);
			}			
		}
	}	
}

void CCELayerTouch::checkHold(float dt)
{
	bool hashold = false;
	long tm = CCEAppUtil::timeTick();
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(item->focusTime>0)
		{
			hashold = true;
			if(!item->holded)
			{
				if(tm - item->focusTime >= item->holdThreshold)
				{
					item->holded = true;
					CCETouchEvent event(m_lastTouch);
					item->node->raiseEvent(NODE_EVENT_HOLD,&event);
				}
			}
		}
	}		
	if(!hashold) {
		m_checkHold = false;
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(CCELayerTouch::checkHold),this);
	}
}

void CCELayerTouch::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
	// CCLOG("ccTouchMoved - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);

	CCPoint cp =touch->getLocation();
	checkItems(&cp);
	m_lastTouch = cp;
}

void CCELayerTouch::orderItems(std::list<CCELayerTouchItem*>& ret)
{
	if(ret.size()>0) {
		std::list<CCELayerTouchItem*>::const_iterator it;
		ret.sort(CCELayerTouchItem());
		bool cover = false;
		for(it=ret.begin();it!=ret.end();)
		{
			std::list<CCELayerTouchItem*>::const_iterator cur = it;
			it++;
			CCELayerTouchItem* pItem = (*cur);
			if(cover) {
				ret.erase(cur);
			} else {
				if(pItem->cover)cover = true;
			}
		}
	}
}

void CCELayerTouch::itemTrackHandler(CCNode* node, const char* name, CCNodeEvent*)
{
	checkItems();
}

CCELayerTouch* CCELayerTouch::getTouchLayer(CCNode* node)
{
    CCNode* pParent = node->getParent();
    for( CCNode *c = pParent; c != NULL; c = c->getParent() )
    {
		CCELayerTouch* r = dynamic_cast<CCELayerTouch*>(c);        
		if(r)return r;
    }
    return NULL;
}

void CCELayerTouch::itemsForTouch(std::list<CCELayerTouchItem*>& ret,CCPoint& touchLocation)
{	
	if (m_touchItems.size()>0)
    {
		std::list<CCELayerTouchItem*>::const_iterator it;
		for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
        {
            CCELayerTouchItem* pItem = (*it);
			CCNode* pChild = pItem->node;
            if (pChild && pChild->isVisible())
            {
                CCPoint local = pChild->convertToNodeSpace(touchLocation);
				if(pChild->containsPoint(local))
                {
					ret.push_front(pItem);					
                }
            }
        }
		if(ret.size()>0) {
			orderItems(ret);			
		}
    }	
}

void CCELayerTouch::getFocusedItems(std::list<CCELayerTouchItem*>& ret)
{	
	if (m_touchItems.size()>0)
    {
		std::list<CCELayerTouchItem*>::const_iterator it;
		for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
        {
            CCELayerTouchItem* pItem = (*it);
			if(pItem->focus) {
				ret.push_front(pItem);
			}
        }
    }	
}
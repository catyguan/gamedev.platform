#include "CCELayerTouch.h"
#include "CCDirector.h"
#include "CCApplication.h"
#include "support/CCPointExtension.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "touch_dispatcher/CCTouch.h"
#include "base_nodes/CCNode_Events.h"

//
//CCELayerTouchItem
//
CCELayerTouchItem::CCELayerTouchItem()
{
	node = NULL;
	
	priority =0;	
    cover = true;
	track = false;

	focus = false;
	tracked = false;
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
{
	m_bEnabled = true;
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

	m_bEnabled = true;       
    
	return true;
}

void CCELayerTouch::onExit()
{
	removeAllTouch();
    CCLayer::onExit();
}

CCELayerTouchItem* CCELayerTouch::createTouch(CCNode* node,bool track)
{
	CC_ASSERT(node);

	CCELayerTouchItem* item = getTouch(node);
	if(item!=NULL)return item;
	item = new CCELayerTouchItem();
	item->node = node;
	node->retain();
	item->track = track;
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
			untrack(pItem);
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
		untrack(pItem);
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

	m_lastTouch = touch->getLocation();

	std::list<CCELayerTouchItem*> items;	
	if (this->itemsForTouch(items,m_lastTouch,true))
	{
		focus(items,m_lastTouch);
		return true;
	}
	return false;
}

void CCELayerTouch::lostFocus(CCELayerTouchItem* item, CCPoint touch)
{
	item->focus = false;
	CCETouchEvent event(touch);
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
		untrack(item);
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
		untrack(item);		
	}
}

void CCELayerTouch::untrack(CCELayerTouchItem* item)
{
	if(item->tracked) {
		item->node->removeEventHandler(NULL,this);
		item->tracked = false;
	}
}

void CCELayerTouch::checkItems()
{
	std::list<CCELayerTouchItem*> items;	        
	this->itemsForTouch(items, m_lastTouch, true);
	focus(items,m_lastTouch);

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
			if(!f)lostFocus(item,m_lastTouch);
		}
	}
}

void CCELayerTouch::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
	CCLOG("ccTouchMoved - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);

	m_lastTouch = touch->getLocation();
	checkItems();
}

bool CCELayerTouch::itemsForTouch(std::list<CCELayerTouchItem*>& ret,CCPoint& touchLocation,bool track)
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
			if(track) {
				if(pItem->track && !pItem->tracked) {
					pChild->onEvent(NODE_EVENT_MOVE,this, nodeevent_selector(CCELayerTouch::itemTrackHandler));
					pItem->tracked = true;
				}
			}
        }
		if(ret.size()>0) {
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
			return true;
		}
    }	
    return false;
}

void CCELayerTouch::itemTrackHandler(CCNode* node, const char* name, CCNodeEvent*)
{
	checkItems();
}

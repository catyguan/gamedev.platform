#include "CCELayerTouch.h"
#include "CCDirector.h"
#include "CCApplication.h"
#include "support/CCPointExtension.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "touch_dispatcher/CCTouch.h"

//
//CCELayerTouchItem
//
CCELayerTouchItem::CCELayerTouchItem()
{
	node = NULL;
	
	priority =0;
	focus = false;
    cover = true;
}

CCELayerTouchItem::~CCELayerTouchItem()
{
	clear();
	CC_SAFE_RELEASE(node);	
}

bool CCELayerTouchItem::hasHandler(int type)
{
	std::list<CCELayerTouchHandler*>::const_iterator it;
	for(it=eventHandlers.begin();it!=eventHandlers.end();it++) {
		CCELayerTouchHandler* h = (*it);
		if(h->type==type) {
			return true;
		}
	}
	return false;
}

void CCELayerTouchItem::addEventHandler(int id, int type, CCObject* obj, SEL_TouchHandler handler)
{
	CCELayerTouchHandler* h = new CCELayerTouchHandler();
	h->id = id;
	h->type = type;
	h->handleObject = obj;
	CC_SAFE_RETAIN(obj);
	h->handler = handler;
	eventHandlers.push_back(h);
}

bool CCELayerTouchItem::remove(int id)
{
	std::list<CCELayerTouchHandler*>::const_iterator it;
	for(it=eventHandlers.begin();it!=eventHandlers.end();it++) {
		CCELayerTouchHandler* h = (*it);
		if(h->id==id) {
			eventHandlers.erase(it);
			CC_SAFE_RELEASE(h->handleObject);
			delete h;			
			return true;
		}		
	}
	return false;
}

void CCELayerTouchItem::clear()
{
	std::list<CCELayerTouchHandler*>::const_iterator it;
	for(it=eventHandlers.begin();it!=eventHandlers.end();it++) {
		CCELayerTouchHandler* h = (*it);
		CC_SAFE_RELEASE(h->handleObject);
		delete h;
	}
	eventHandlers.clear();
}

void CCELayerTouchItem::call(int checktype,int type, CCTouch* touch)
{
	std::list<CCELayerTouchHandler*>::const_iterator it;
	for(it=eventHandlers.begin();it!=eventHandlers.end();it++) {
		CCELayerTouchHandler* h = (*it);
		if(h->type==checktype) {
			(h->handleObject->*h->handler)(node, type, touch);
		}
	}
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

CCELayerTouchItem* CCELayerTouch::createTouch(CCNode* node)
{
	CC_ASSERT(node);

	CCELayerTouchItem* item = getTouch(node);
	if(item!=NULL)return item;
	item = new CCELayerTouchItem();
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
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
    {
        CCELayerTouchItem* pItem = (*it);
		if(pItem->node==node) {
			m_touchItems.erase(it);
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

void CCELayerTouch::focus(std::list<CCELayerTouchItem*>& items, CCTouch* touch)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=items.begin();it!=items.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(!item->focus) {
			item->focus = true;
			item->call(kTouchTypeFocus,touch);				
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

	std::list<CCELayerTouchItem*> items;	
    if (this->itemsForTouch(items,touch))
	{
		focus(items,touch);
		return true;
	}
	return false;
}

void CCELayerTouch::lostFocus(CCELayerTouchItem* item, CCTouch* touch)
{
	item->focus = false;
	item->call(kTouchTypeFocus,kTouchTypeUnFocus,touch);	
}

void CCELayerTouch::ccTouchEnded(CCTouch *touch, CCEvent* event)
{    
    CC_UNUSED_PARAM(event);
	CCLOG("ccTouchEnded - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++) {
		CCELayerTouchItem* item = (*it);
		if(item->focus) {
			item->call(kTouchTypeClick, touch);			
			lostFocus(item, touch);
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
			lostFocus(item, touch);
		}
	}
}

void CCELayerTouch::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
	CCLOG("ccTouchMoved - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);

	
	std::list<CCELayerTouchItem*> items;	        
    this->itemsForTouch(items,touch);
	focus(items,touch);

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
			if(!f)lostFocus(item,touch);
		}
	}
}

bool CCELayerTouch::itemsForTouch(std::list<CCELayerTouchItem*>& ret,CCTouch * touch)
{	
    CCPoint touchLocation = touch->getLocation();

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
				CCRect r = pChild->boundingBox();
                r.origin = CCPointZero;
                if (r.containsPoint(local))
                {
					ret.push_front(pItem);					
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

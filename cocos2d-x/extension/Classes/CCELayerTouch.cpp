#include "CCELayerTouch.h"
#include "CCDirector.h"
#include "CCApplication.h"
#include "support/CCPointExtension.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "touch_dispatcher/CCTouch.h"
#include "base_nodes/CCNode_Events.h"
#include "CCEAppUtil.h"
#include "cocoa\CCValueSupport.h"
#include "CCEUtil.h"

//
//CCELayerTouchItem
//
CCELayerTouchItem::CCELayerTouchItem()
	: recognizer(NULL)
	, priority(0)
	, actived(false)
{
}

CCELayerTouchItem::~CCELayerTouchItem()
{
	CC_SAFE_DELETE(recognizer);
}

bool CCELayerTouchItem::operator() (const CCELayerTouchItem* t1,const CCELayerTouchItem* t2)
{
	if(t1->priority > t2->priority) return true;
	CCEGestureRecognizer4Node* p1 = dynamic_cast<CCEGestureRecognizer4Node*>(t1->recognizer);	
	if(p1!=NULL) {
		CCEGestureRecognizer4Node* p2 = dynamic_cast<CCEGestureRecognizer4Node*>(t2->recognizer);
		if(p2!=NULL) {
			if(p1->getNode()->getZOrder() > p2->getNode()->getZOrder())return true;
		}
	}
	return false;
}

//
//CCELayerTouch
//
CCELayerTouch::CCELayerTouch()
	: m_bEnabled(true), m_traveling(false)
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
}

void CCELayerTouch::onExit()
{
	removeAllTouch();
    CCLayer::onExit();
}

void CCELayerTouch::reorderItems()
{
	sortItems(m_touchItems);
	sortItems(m_activedItems);
}

void CCELayerTouch::sortItems(std::list<CCELayerTouchItem*>& ret)
{
	if(ret.size()>0) {
		ret.sort(CCELayerTouchItem());
	}
}


CCELayerTouchItem* CCELayerTouch::createTouch(CCEGestureRecognizer* recognizer)
{
	CC_ASSERT(recognizer);

	CCELayerTouchItem* item = getTouch(recognizer);
	if(item!=NULL)return item;
	item = new CCELayerTouchItem();
	item->recognizer = recognizer;
	m_touchItems.push_back(item);
	reorderItems();
	return item;
}

CCELayerTouchItem* CCELayerTouch::getTouch(CCEGestureRecognizer* recognizer)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
    {
        CCELayerTouchItem* pItem = (*it);
		if(pItem->recognizer==recognizer)return pItem;
    }
	return NULL;
}

CCELayerTouchItem* CCELayerTouch::getTouchByNode(CCNode* node)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
    {
        CCELayerTouchItem* pItem = (*it);
		CCEGestureRecognizer4Node* p = dynamic_cast<CCEGestureRecognizer4Node*>(pItem->recognizer);
		if(p!=NULL && p->getNode()==node)return pItem;
    }
	return NULL;
}

void CCELayerTouch::recognized(int id, CCPoint pt, CCELayerTouchItem* item)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	for(it=m_activedItems.begin();it!=m_activedItems.end();)
	{
		std::list<CCELayerTouchItem*>::const_iterator cur = it;
		it++;
		CCELayerTouchItem* pItem = (*cur);
		if(pItem!=item) {
			pItem->actived = false;
			m_activedItems.erase(cur);
			pItem->recognizer->touchCancelled(id, pt);
		}
	}
}

void CCELayerTouch::addActiveItem(CCELayerTouchItem* item)
{
	if(!item->actived) {
		item->actived = true;		
		m_activedItems.push_back(item);
	}	
}

void CCELayerTouch::removeActiveItem(CCELayerTouchItem* item,bool except)
{
	std::list<CCELayerTouchItem*>::const_iterator it,cur;
	for(it=m_activedItems.begin();it!=m_activedItems.end();)
    {
		cur = it;
		it++;
        CCELayerTouchItem* pItem = (*cur);
		bool r = false;
		if(pItem==item) {
			if(!except) {
				r = true;
			}
		} else {
			if(except) {
				r = true;
			}
		}
		if(r) {
			pItem->actived = false;
			m_activedItems.erase(cur);
			break;
		}
    }
}

void CCELayerTouch::removeItem(std::list<CCELayerTouchItem*>::const_iterator it)
{
	CCELayerTouchItem* pItem = *it;
	m_touchItems.erase(it);
	removeActiveItem(pItem,false);
	delete pItem;
}

void CCELayerTouch::removeTouch(CCEGestureRecognizer* recognizer)
{
	std::list<CCELayerTouchItem*>::const_iterator it,cur;
	for(it=m_touchItems.begin();it!=m_touchItems.end();)
    {
		cur = it;
		it++;
        CCELayerTouchItem* pItem = (*cur);
		if(pItem->recognizer==recognizer) {
			if(m_traveling) {
				m_removedItems.push_back(pItem);
			} else {
				removeItem(cur);
			}
		}
    }
}

void CCELayerTouch::removeTouchByNode(CCNode* node)
{
	std::list<CCELayerTouchItem*>::const_iterator it,cur;
	for(it=m_touchItems.begin();it!=m_touchItems.end();)
    {
		cur = it;
		it++;
        CCELayerTouchItem* pItem = (*cur);
		CCEGestureRecognizer4Node* p = dynamic_cast<CCEGestureRecognizer4Node*>(pItem->recognizer);
		if(p!=NULL && p->getNode()==node) {
			if(m_traveling) {
				m_removedItems.push_back(pItem);
			} else {
				removeItem(cur);
			}
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
	m_activedItems.clear();
	m_removedItems.clear();
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

void CCELayerTouch::travel(int type,int id, CCPoint pt)
{
	m_traveling = true;
	_travel(type, id, pt);
	m_traveling = false;
	if(m_removedItems.size()>0) {		
		std::list<CCELayerTouchItem*>::const_iterator it;
		for(it=m_removedItems.begin();it!=m_removedItems.end();it++)
		{
			CCELayerTouchItem* item = *it;

			std::list<CCELayerTouchItem*>::const_iterator it2;
			for(it2=m_touchItems.begin();it2!=m_touchItems.end();)
			{
				std::list<CCELayerTouchItem*>::const_iterator cur = it2;
				it2++;
				if(*cur==item) {
					removeItem(cur);
				}
			}			
		}
	}
}

void CCELayerTouch::_travel(int type,int id, CCPoint pt)
{
	std::list<CCELayerTouchItem*>::const_iterator it;
	if(m_activedItems.size()>0) {		
		for(it=m_activedItems.begin();it!=m_activedItems.end();)
		{
			std::list<CCELayerTouchItem*>::const_iterator cur = it;
			it++;
			CCELayerTouchItem* pItem = (*cur);
			bool t = false;
			switch(type) {
			case 1:t = pItem->recognizer->touchBegan(id, pt);break;
			case 2:t = pItem->recognizer->touchMoved(id, pt);break;
			case 3:t = pItem->recognizer->touchEnded(id, pt);break;
			case 4:pItem->recognizer->touchCancelled(id, pt);break;
			}
			if(!t) {
				pItem->actived = false;
				m_activedItems.erase(cur);
			}
			if(pItem->recognizer->isRecognized()) {
				recognized(id, pt, pItem);
				return;
			}
		}		
	}

	if(m_activedItems.size()==0) {
		std::list<CCELayerTouchItem*>::const_iterator it;
		for(it=m_touchItems.begin();it!=m_touchItems.end();it++)
		{
			CCELayerTouchItem* pItem = (*it);
			bool t = false;
			switch(type) {
			case 1:t = pItem->recognizer->touchBegan(id, pt);break;
			case 2:t = pItem->recognizer->touchMoved(id, pt);break;
			case 3:t = pItem->recognizer->touchEnded(id, pt);break;
			case 4:pItem->recognizer->touchCancelled(id, pt);break;
			}			
			if(t) {
				addActiveItem(pItem);
			}
			if(pItem->recognizer->isRecognized()) {
				recognized(id, pt, pItem);
				return;
			}			
		}
	}
}

void CCELayerTouch::checkItems(int* id, CCPoint* pt)
{
	if(id==NULL)id=&m_lastId;
	if(pt==NULL)pt = &m_lastTouch;
	travel(2, *id, *pt);
}

bool CCELayerTouch::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
	// CCLOG("ccTouchBegan - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);
	
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
	travel(1, touch->getID(), cp);
	m_lastId = touch->getID();
	m_lastTouch = cp;
	return true;
}

void CCELayerTouch::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
	// CCLOG("ccTouchMoved - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);

	CCPoint cp =touch->getLocation();
	travel(2, touch->getID(), cp);
	m_lastId = touch->getID();
	m_lastTouch = cp;
}

void CCELayerTouch::ccTouchEnded(CCTouch *touch, CCEvent* event)
{    
    CC_UNUSED_PARAM(event);
	// CCLOG("ccTouchEnded - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);

	CCPoint cp = touch->getLocation();
	travel(3, touch->getID(), cp);
	m_lastId = touch->getID();
	m_lastTouch = cp;	
}

void CCELayerTouch::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{    
    CC_UNUSED_PARAM(event);
	// CCLOG("ccTouchCancelled - %d (%f, %f)", touch->getID(), touch->getLocation().x, touch->getLocation().y);

	CCPoint cp = touch->getLocation();
	travel(4, touch->getID(), cp);
	m_lastId = touch->getID();
	m_lastTouch = cp;	
}

CCELayerTouch* CCELayerTouch::getTouchLayer(CCNode* node)
{
	while(node!=NULL) {
		CCELayerTouch* r = dynamic_cast<CCELayerTouch*>(node);
		if(r!=NULL)return r;
		node = node->getParent();
	}
	return NULL;
}

CC_BEGIN_CALLS(CCELayerTouch, CCLayer)	
	CC_DEFINE_CALL(CCELayerTouch, enableTouch)
	CC_DEFINE_CALL(CCELayerTouch, disableTouch)
CC_END_CALLS(CCELayerTouch, CCLayer)

CCValue CCELayerTouch::CALLNAME(enableTouch)(CCValueArray& params) {
	CCNode* node = NULL;
	if(params.size()>0) {
		if(params[0].isString()) {
			node = (CCNode*) findChildById(params[0].stringValue().c_str());
		} else {
			node = ccvpObject(params,0,CCNode);
		}
	}
	std::string ename = ccvpString(params, 1);
	int p = -1;

	if(node==NULL) {
		throw new std::string("param 1 expect CCNode");
	}
	if(ename.size()==0) {
		throw new std::string("param 2 event name");
	}
	if(params.size()>2) {
		p = params[2].intValue();
	}
	CCETouchBuilder b;
	b.bind(node).on(ename);
	if(p>=0) {
		b.setPriority(p);
	}
	b.createTouch(this);
	return CCValue::nullValue();
}
CCValue CCELayerTouch::CALLNAME(disableTouch)(CCValueArray& params) {
	CCNode* node = NULL;
	if(params.size()>0) {
		if(params[0].isString()) {
			node = (CCNode*) findChildById(params[0].stringValue().c_str());
		} else {
			node = ccvpObject(params,0,CCNode);
		}
	}
	if(node!=NULL) {
		removeTouchByNode(node);
	}	
	return CCValue::nullValue();
}

//
// CCETouchBuilder
// 
CCETouchBuilder::CCETouchBuilder()
	: node(NULL)
	, priority(0)
{

}

CCETouchBuilder::~CCETouchBuilder()
{
	clear(true);
}

void CCETouchBuilder::clear(bool del)
{	
	if(del) {
		std::list<CCEGestureRecognizer*>::const_iterator it;
		for(it=recognizers.begin();it!=recognizers.end();it++) {
			delete *it;
		}	
	}
	recognizers.clear();
	node = NULL;
}

CCETouchBuilder& CCETouchBuilder::addGestureRecognizer(CCEGestureRecognizer* obj)
{
	recognizers.push_back(obj);
	return *this;
}

CCETouchBuilder& CCETouchBuilder::bind(CCNode* n)
{
	clear(true);
	node = n;
	return *this;
}

CCETouchBuilder& CCETouchBuilder::on(std::string type)
{
	std::string delim(",");
	std::vector<std::string> list;
	StringUtil::split(type, delim, &list);
	if(list.size()>0) {
		for(size_t i=0;i<list.size();i++) {
			std::string type = StringUtil::trim(list[i]);
			if(type.compare("tap")==0) {
				onTap(NULL,NULL);
			} else if(type.compare("focus")==0) {
				onFocus(NULL,NULL);
			} else if(type.compare("holdpress")==0) {
				onHoldpress(NULL,NULL);
			}
		}
	}
	return *this;
}

CCETouchBuilder& CCETouchBuilder::onFocus(CCObject* obj,SEL_NodeEventHandler handler)
{
	CC_ASSERT(node!=NULL);
	CCEGestureRecognizer4Focus* gr = CCEGestureRecognizer4Focus::create(node);
	if(obj!=NULL && handler!=NULL) {
		node->onEvent(NODE_EVENT_FOCUS, obj, handler);
	}
	addGestureRecognizer(gr);
	return *this;
}

CCETouchBuilder& CCETouchBuilder::onTap(CCObject* obj,SEL_NodeEventHandler handler)
{
	CC_ASSERT(node!=NULL);	
	CCEGestureRecognizer4Tap* gr = CCEGestureRecognizer4Tap::create(node);
	if(obj!=NULL && handler!=NULL) {
		node->onEvent(NODE_EVENT_TAP, obj, handler);
	}
	addGestureRecognizer(gr);
	return *this;
}

CCETouchBuilder& CCETouchBuilder::onHoldpress(CCObject* obj,SEL_NodeEventHandler handler)
{
	CC_ASSERT(node!=NULL);	
	CCEGestureRecognizer4Holdpress* gr = CCEGestureRecognizer4Holdpress::create(node);
	if(obj!=NULL && handler!=NULL) {
		node->onEvent(NODE_EVENT_HOLDPRESS, obj, handler);
	}
	addGestureRecognizer(gr);
	return *this;
}

CCETouchBuilder& CCETouchBuilder::onHoldpress(CCObject* obj,SEL_NodeEventHandler handler,int timeThreshold, int moveThreshold, float checkInterval)
{
	CC_ASSERT(node!=NULL);	
	CCEGestureRecognizer4Holdpress* gr = CCEGestureRecognizer4Holdpress::create(node,timeThreshold,moveThreshold,checkInterval);
	if(obj!=NULL && handler!=NULL) {
		node->onEvent(NODE_EVENT_HOLDPRESS, obj, handler);
	}
	addGestureRecognizer(gr);
	return *this;
}


void CCETouchBuilder::createTouch(CCELayerTouch* layer)
{
	if(layer==NULL) {
		layer = CCELayerTouch::getTouchLayer(node);
	}
	std::list<CCEGestureRecognizer*>::const_iterator it;
	for(it=recognizers.begin();it!=recognizers.end();it++) {
		CCELayerTouchItem* item = layer->createTouch(*it);
		item->setPriority(priority);
	}
	clear(false);
}
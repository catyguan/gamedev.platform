#include "CCELogoScene.h"
#include "CCEAction.h"

USING_NS_CC;

CCELogoLayer::CCELogoLayer()
{

}

CCELogoLayer::~CCELogoLayer()
{

}

CCELogoLayer* CCELogoLayer::create(CCValueArray& ps)
{
	CCELogoLayer* layer = new CCELogoLayer();	
	if(!layer->init()) {
		CC_SAFE_DELETE(layer);
		return NULL;
	}
	if(ps.size()>0) {
		layer->m_callDone = ps[0];
		layer->m_callDone.retain();
	}
	layer->autorelease();
	return layer;
}

bool CCELogoLayer::init()
{
    // super init first
	if(!CCELayerTouch::init()) {    
        return false;
    }
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCLayerColor* l = CCLayerColor::create(ccc4(255,255,255,255));
	this->addChild(l);

	CCELayerTouchItem* item = createTouch(l);
	item->onClick(1,this,touch_selector(CCELogoLayer::clickHandler));

    // build logo
    CCSprite* logo1 = CCSprite::create("logo_me.png");
	// position the sprite on the center of the screen
	logo1->setPosition(ccp(winSize.width/2, winSize.height/2));
	this->addChild(logo1);

	CCSprite* logo2 = CCSprite::create("logo_cocos2dx.png");
    logo2->setPosition(ccp(winSize.width/2, winSize.height/2));
	logo2->setVisible(false);
	this->addChild(logo2);

	// actions
	CCArray* arr = CCArray::createWithCapacity(5);
	CCDelayTime* a1 = CCDelayTime::create(2.5);
	arr->addObject(a1);
	CCECall* a2 = CCECall::create(logo1,"visible",CCValue::booleanValue(false));
	arr->addObject(a2);
	CCECall* a3 = CCECall::create(logo2,"visible",CCValue::booleanValue(true));
	arr->addObject(a3);
	CCDelayTime* a4 = CCDelayTime::create(1.5);	
	arr->addObject(a4);
	CCECall* a5 = CCECall::create(logo2,"visible",CCValue::booleanValue(false));
	arr->addObject(a5);
	if(m_callDone.canCall()) {		
		CCECall* a6 = CCECall::create(m_callDone);
		arr->addObject(a6);
	}
	CCSequence* seq = CCSequence::create(arr);
	seq->setTag(1);
	this->runAction(seq);
    
    return true;
}

void CCELogoLayer::clickHandler(CCNode* node, int type, CCTouch*)
{
	this->stopActionByTag(1);
	if(m_callDone.canCall()) {
		CCValueArray ps;
		m_callDone.call(ps,false);
	}
}

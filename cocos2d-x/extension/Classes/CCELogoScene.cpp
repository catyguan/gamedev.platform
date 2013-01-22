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
	CCValue call;
	if(ps.size()>0) {
		call = ps[0];		
	}
	if(!layer->init(call)) {
		CC_SAFE_DELETE(layer);
		return NULL;
	}	
	layer->autorelease();
	return layer;
}

bool CCELogoLayer::init(CCValue call)
{
    // super init first
	if(!CCELayerTouch::init()) {    
        return false;
    }

	m_callDone = call;
	m_callDone.retain();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCLayerColor* l = CCLayerColor::create(ccc4(255,255,255,255));
	this->addChild(l);

	CCELayerTouchItem* item = createTouch(l);
	l->onEvent(NODE_EVENT_CLICK,this,nodeevent_selector(CCELogoLayer::clickHandler));

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

void CCELogoLayer::clickHandler(CCNode* node, const char* name, CCNodeEvent*)
{
	this->stopActionByTag(1);
	if(m_callDone.canCall()) {
		CCValueArray ps;
		m_callDone.call(ps,false);
	}
}

#include "CCEDEVUpgradeConfigScene.h"
#include "CCELayerTouch.h"
#include "CCEButton.h"
#include "CCEUtil.h"
#include "editbox\CCEditBox.h"
#include "CCEUpgradeManager.h"

USING_NS_CC;

// CCEDEVUpgradeScene
CCEDEVUpgradeConfigScene::CCEDEVUpgradeConfigScene()
{

}

CCEDEVUpgradeConfigScene::~CCEDEVUpgradeConfigScene()
{

}

bool CCEDEVUpgradeConfigScene::init()
{
	if(!CCEScene::init())return false;

	UpgradeConfig cfg;
	CCEUpgradeManager::sharedUpgradeManager()->getConfig(&cfg);

	CCELayerTouch* layer = CCELayerTouch::create();
	layer->setTag(1);
	this->addChild(layer);

	CCSize sz = CCDirector::sharedDirector()->getWinSize();
	float x = 100;
	float y = sz.height - 100;
	float h = 40;
	
	const char* fontName = "Arial";
	CCLabelTTF* l1 = CCLabelTTF::create("url: ", fontName, 24);
	l1->setAnchorPoint(CCPointZero);
	l1->setPosition(ccp(x,y));
	h = l1->getContentSize().height;
	layer->addChild(l1);

	CCEditBox* e1 = CCEditBox::create(CCSizeMake(600,h));
	e1->setFontName(fontName);
	e1->setFontSize(24);
	e1->setAnchorPoint(ccp(0, 0));
	e1->setPosition(ccp(l1->getContentSize().width+x, y));	
	e1->setText(cfg.url.c_str());
	y -= (e1->getContentSize().height + 16);
	layer->addChild(e1);

	CCLabelTTF* l2 = CCLabelTTF::create("host: ", fontName, 24);
	l2->setAnchorPoint(CCPointZero);
	l2->setPosition(ccp(x,y));
	layer->addChild(l2);

	CCEditBox* e2 = CCEditBox::create(CCSizeMake(600,h));
	e2->setFontName(fontName);
	e2->setFontSize(24);
	e2->setAnchorPoint(ccp(0, 0));
	e2->setPosition(ccp(l2->getContentSize().width+x, y));	
	e2->setText(cfg.host.c_str());
	y -= (e2->getContentSize().height + 16);
	layer->addChild(e2);

	CCLabelTTF* l3 = CCLabelTTF::create("current version: ", fontName, 24);
	l3->setAnchorPoint(CCPointZero);
	l3->setPosition(ccp(x,y));
	layer->addChild(l3);

	CCEditBox* e3 = CCEditBox::create(CCSizeMake(600,h));
	e3->setFontName(fontName);
	e3->setFontSize(24);
	e3->setAnchorPoint(ccp(0, 0));
	e3->setPosition(ccp(l3->getContentSize().width+x, y));	
	e3->setText(cfg.version.c_str());
	y -= (e3->getContentSize().height + 16);
	layer->addChild(e3);

	y -= 100;
	CCLabelTTF* lb1 = CCLabelTTF::create("GoBack ", "Arial", 32);
	CCEButton* b1 = CCEButton::create(lb1, NULL);
	b1->setAnchorPoint(ccp(0,0.5));
	b1->setPosition(ccp(x,y));
	y -= (b1->getContentSize().height + 16);
	layer->addChild(b1);

	CCETouchBuilder b;
	b.bind(e1).on("tap").createTouch(layer);
	b.bind(e2).on("tap").createTouch(layer);
	b.bind(e3).on("tap").createTouch(layer);

	b.bind(b1).onTap(this, nodeevent_selector(CCEDEVUpgradeConfigScene::onBackClick));
	b.createTouch(layer);

	e1->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVUpgradeConfigScene::onUrlChanged));
	e2->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVUpgradeConfigScene::onHostChanged));
	e3->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVUpgradeConfigScene::onVersionChanged));

	return true;
}

CCEDEVUpgradeConfigScene* CCEDEVUpgradeConfigScene::create()
{
	CCEDEVUpgradeConfigScene* r = new CCEDEVUpgradeConfigScene();
	if(r->init()) {
		r->autorelease();
		return r;
	}
	CC_SAFE_DELETE(r);
	return NULL;
}

void CCEDEVUpgradeConfigScene::onBackClick(CCNode* node, const char* name, CCNodeEvent*)
{	
	CCDirector::sharedDirector()->popScene();
}

void CCEDEVUpgradeConfigScene::onUrlChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEUpgradeManager* m = CCEUpgradeManager::sharedUpgradeManager();
	UpgradeConfig cfg;
	m->getConfig(&cfg);
	if(cfg.url.compare(s)!=0) {
		cfg.url = s;
		m->setConfig(&cfg);
	}
}


void CCEDEVUpgradeConfigScene::onHostChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEUpgradeManager* m = CCEUpgradeManager::sharedUpgradeManager();
	UpgradeConfig cfg;
	m->getConfig(&cfg);
	if(cfg.host.compare(s)!=0) {
		cfg.host = s;
		m->setConfig(&cfg);
	}
}


void CCEDEVUpgradeConfigScene::onVersionChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEUpgradeManager* m = CCEUpgradeManager::sharedUpgradeManager();
	UpgradeConfig cfg;
	m->getConfig(&cfg);
	if(cfg.version.compare(s)!=0) {
		cfg.version = s;
		m->setConfig(&cfg);
	}
}
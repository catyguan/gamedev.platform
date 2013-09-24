#include "CCEDEVAppConfigScene.h"
#include "CCELayerTouch.h"
#include "CCEButton.h"
#include "CCEUtil.h"
#include "editbox\CCEditBox.h"
#include "CCEDEVConst.h"
#include "CCEAppUtil.h"

USING_NS_CC;

// CCEDEVAppConfigScene
CCEDEVAppConfigScene::CCEDEVAppConfigScene()
{

}

CCEDEVAppConfigScene::~CCEDEVAppConfigScene()
{

}

bool CCEDEVAppConfigScene::init()
{
	if(!CCEScene::init())return false;

	CCEAppConfig cfg;
	CCEAppUtil::readConfig(&cfg);

	CCELayerTouch* layer = CCELayerTouch::create();
	layer->setTag(1);
	this->addChild(layer);

	CCSize sz = CCDirector::sharedDirector()->getWinSize();
	float x = 100;
	float y = sz.height - 100;
	float h = 40;
	
	const char* fontName = DEV_FONT_NAME;
	CCLabelTTF* l1 = CCLabelTTF::create("LuaApp ID: ", fontName, DEV_LABEL_FONT_SIZE);
	l1->setAnchorPoint(CCPointZero);
	l1->setPosition(ccp(x,y));
	h = l1->getContentSize().height;
	layer->addChild(l1);

	CCEditBox* e1 = CCEditBox::create(CCSizeMake(600,h));
	e1->setFontName(fontName);
	e1->setFontSize(DEV_EDITOR_FONT_SIZE);
	e1->setAnchorPoint(ccp(0, 0));
	e1->setPosition(ccp(l1->getContentSize().width+x, y));	
	e1->setText(cfg.appId.c_str());
	y -= (e1->getContentSize().height + 16);
	layer->addChild(e1);

	CCLabelTTF* l2 = CCLabelTTF::create("Design Width: ", fontName, DEV_LABEL_FONT_SIZE);
	l2->setAnchorPoint(CCPointZero);
	l2->setPosition(ccp(x,y));
	layer->addChild(l2);

	CCEditBox* e2 = CCEditBox::create(CCSizeMake(600,h));
	e2->setFontName(fontName);
	e2->setFontSize(DEV_EDITOR_FONT_SIZE);
	e2->setAnchorPoint(ccp(0, 0));
	e2->setPosition(ccp(l2->getContentSize().width+x, y));	
	e2->setText(StringUtil::format("%d",cfg.designWidth).c_str());
	y -= (e2->getContentSize().height + 16);
	layer->addChild(e2);

	CCLabelTTF* l3 = CCLabelTTF::create("Design Height: ", fontName, DEV_LABEL_FONT_SIZE);
	l3->setAnchorPoint(CCPointZero);
	l3->setPosition(ccp(x,y));
	layer->addChild(l3);

	CCEditBox* e3 = CCEditBox::create(CCSizeMake(600,h));
	e3->setFontName(fontName);
	e3->setFontSize(DEV_EDITOR_FONT_SIZE);
	e3->setAnchorPoint(ccp(0, 0));
	e3->setPosition(ccp(l3->getContentSize().width+x, y));	
	e3->setText(StringUtil::format("%d",cfg.designHeight).c_str());
	y -= (e3->getContentSize().height + 16);
	layer->addChild(e3);

	CCLabelTTF* l4 = CCLabelTTF::create("Design Policy: ", fontName, DEV_LABEL_FONT_SIZE);
	l4->setAnchorPoint(CCPointZero);
	l4->setPosition(ccp(x,y));
	layer->addChild(l4);

	CCEditBox* e4 = CCEditBox::create(CCSizeMake(600,h));
	e4->setFontName(fontName);
	e4->setFontSize(DEV_EDITOR_FONT_SIZE);
	e4->setAnchorPoint(ccp(0, 0));
	e4->setPosition(ccp(l4->getContentSize().width+x, y));	
	e4->setText(StringUtil::format("%d", cfg.designPolicy).c_str());
	y -= (e4->getContentSize().height + 16);
	layer->addChild(e4);

	CCLabelTTF* lx = CCLabelTTF::create("NOTICE: restart MGR after change configs", fontName, DEV_LABEL_FONT_SIZE);
	lx->setColor(ccc3(255,0,0));
	lx->setAnchorPoint(CCPointZero);
	lx->setPosition(ccp(x,y));
	layer->addChild(lx);
	y -= (lx->getContentSize().height + 16);

	y -= 100;
	CCLabelTTF* lb1 = CCLabelTTF::create("GoBack ", DEV_FONT_NAME, DEV_LABEL_FONT_SIZE);
	CCEButton* b1 = CCEButton::create(lb1, NULL);
	b1->setAnchorPoint(ccp(0,0.5));
	b1->setPosition(ccp(x,y));
	y -= (b1->getContentSize().height + 16);
	layer->addChild(b1);

	CCETouchBuilder b;
	b.bind(e1).on("tap").createTouch(layer);
	b.bind(e2).on("tap").createTouch(layer);
	b.bind(e3).on("tap").createTouch(layer);
	b.bind(e4).on("tap").createTouch(layer);

	b.bind(b1).onTap(this, nodeevent_selector(CCEDEVAppConfigScene::onBackClick));
	b.createTouch(layer);

	e1->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVAppConfigScene::onAppIdChanged));
	e2->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVAppConfigScene::onDesignWidthChanged));
	e3->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVAppConfigScene::onDesignHeightChanged));
	e4->onEvent(NODE_EVENT_EDITBOX_RETURN,this, nodeevent_selector(CCEDEVAppConfigScene::onDesignPolicyChanged));

	return true;
}

CCEDEVAppConfigScene* CCEDEVAppConfigScene::create()
{
	CCEDEVAppConfigScene* r = new CCEDEVAppConfigScene();
	if(r->init()) {
		r->autorelease();
		return r;
	}
	CC_SAFE_DELETE(r);
	return NULL;
}

void CCEDEVAppConfigScene::onBackClick(CCNode* node, const char* name, CCNodeEvent*)
{	
	CCDirector::sharedDirector()->popScene();
}

void CCEDEVAppConfigScene::onAppIdChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEAppConfig cfg;
	CCEAppUtil::readConfig(&cfg);
	if(cfg.appId.compare(s)!=0) {
		cfg.appId = s;
		CCEAppUtil::writeConfig(&cfg);
	}
}


void CCEDEVAppConfigScene::onDesignWidthChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEAppConfig cfg;
	CCEAppUtil::readConfig(&cfg);
	if(s.size()>0 && cfg.designWidth!=atoi(s.c_str())) {
		cfg.designWidth = atoi(s.c_str());
		CCEAppUtil::writeConfig(&cfg);
	}
}

void CCEDEVAppConfigScene::onDesignHeightChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEAppConfig cfg;
	CCEAppUtil::readConfig(&cfg);
	if(s.size()>0 && cfg.designHeight!=atoi(s.c_str())) {
		cfg.designHeight = atoi(s.c_str());
		CCEAppUtil::writeConfig(&cfg);
	}
}

void CCEDEVAppConfigScene::onDesignPolicyChanged(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEditBox* box = (CCEditBox*) node;
	std::string s = box->getText();

	CCEAppConfig cfg;
	CCEAppUtil::readConfig(&cfg);
	if(s.size()>0 && cfg.designPolicy!=atoi(s.c_str())) {
		cfg.designPolicy = atoi(s.c_str());
		CCEAppUtil::writeConfig(&cfg);
	}
}

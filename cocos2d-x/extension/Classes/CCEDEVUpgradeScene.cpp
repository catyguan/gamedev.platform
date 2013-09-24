#include "CCEDEVUpgradeScene.h"
#include "CCELayerTouch.h"
#include "CCEButton.h"
#include "CCEUtil.h"
#include "CCEDEVConst.h"

USING_NS_CC;

// CCEDEVUpgradeScene
CCEDEVUpgradeScene::CCEDEVUpgradeScene()
{

}

CCEDEVUpgradeScene::~CCEDEVUpgradeScene()
{

}

bool CCEDEVUpgradeScene::init()
{
	if(!CCEScene::init())return false;

	CCELayerTouch* layer = CCELayerTouch::create();
	layer->setTag(1);
	this->addChild(layer);

	CCSize sz = CCDirector::sharedDirector()->getWinSize();
	float x = 100;
	float y = sz.height - 100;

	CCLabelTTF* l1 = CCLabelTTF::create("Status: ", DEV_FONT_NAME, DEV_LABEL_FONT_SIZE);
	l1->setTag(1);
	l1->setAnchorPoint(ccp(0, 0.5));
	l1->setPosition(ccp(x, y));
	y -= (l1->getContentSize().height + 16);
	layer->addChild(l1);

	CCLabelTTF* l2 = CCLabelTTF::create("Progress: ", DEV_FONT_NAME, DEV_LABEL_FONT_SIZE);
	l2->setTag(2);
	l2->setAnchorPoint(ccp(0, 0.5));
	l2->setPosition(ccp(x, y));
	y -= (l2->getContentSize().height + 16);
	layer->addChild(l2);
		
	y -= 100;
	CCLabelTTF* l4 = CCLabelTTF::create("Upgrade!", DEV_FONT_NAME, DEV_BUTTON_FONT_SIZE);
	CCEButton* b2 = CCEButton::create(l4, NULL);
	b2->setAnchorPoint(ccp(0,0.5));
	b2->setPosition(ccp(x,y));
	y -= (b2->getContentSize().height + 16);
	layer->addChild(b2);

	CCLabelTTF* l3 = CCLabelTTF::create("GoBack ", DEV_FONT_NAME, DEV_BUTTON_FONT_SIZE);
	CCEButton* b1 = CCEButton::create(l3, NULL);
	b1->setAnchorPoint(ccp(0,0.5));
	b1->setPosition(ccp(x,y));
	y -= (b1->getContentSize().height + 16);
	layer->addChild(b1);

	CCETouchBuilder b;
	b.bind(b1).onTap(this, nodeevent_selector(CCEDEVUpgradeScene::onBackClick));
	b.createTouch(layer);

	b.bind(b2).onTap(this, nodeevent_selector(CCEDEVUpgradeScene::onGradeClick));
	b.createTouch(layer);

	scheduleUpdate();
	
	return true;
}

CCEDEVUpgradeScene* CCEDEVUpgradeScene::create()
{
	CCEDEVUpgradeScene* r = new CCEDEVUpgradeScene();
	if(r->init()) {
		r->autorelease();
		return r;
	}
	CC_SAFE_DELETE(r);
	return NULL;
}

void CCEDEVUpgradeScene::update(float v)
{
	CCEScene::update(v);

	CCEUpgradeManager* m = CCEUpgradeManager::sharedUpgradeManager();
	UpgradeInfo info;
	m->queryInfo(&info);	

	if(m_lastInfo.end==info.end
		&& m_lastInfo.fail==info.fail
		&& m_lastInfo.finishWork==info.finishWork
		&& m_lastInfo.message.compare(info.message)==0
		&& m_lastInfo.totalWork==info.totalWork)
	{
		return;
	}

	CCNode* layer = getChildByTag(1);
	CCLabelTTF* l1 = (CCLabelTTF*) layer->getChildByTag(1);
	CCLabelTTF* l2 = (CCLabelTTF*) layer->getChildByTag(2);

	std::string s1;
	if(m->isProcessing() || info.end) {
		s1 = StringUtil::format("Status: %s", info.end?(info.fail?"fail":"done"):"processing");
		if(info.message.size()>0) {
			s1 += " - ";
			s1 += info.message;
		}
	} else {
		s1 = "Status: not start";
	}
	l1->setString(s1.c_str());

	std::string s2;
	if(!m->isProcessing() || info.totalWork==0) {
		s2 = "Progress: -- ";
	} else {
		s2 = StringUtil::format("Progress: %2.2f", info.finishWork*100.0f/info.totalWork);
	}
	l2->setString(s2.c_str());

	m_lastInfo.end = info.end;
	m_lastInfo.fail = info.fail;
	m_lastInfo.finishWork = info.finishWork;
	m_lastInfo.totalWork = info.totalWork;
	m_lastInfo.message = info.message;
}

void CCEDEVUpgradeScene::onBackClick(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEUpgradeManager::sharedUpgradeManager()->cancel();
	CCDirector::sharedDirector()->popScene();
}

void CCEDEVUpgradeScene::onGradeClick(CCNode* node, const char* name, CCNodeEvent*)
{
	CCEUpgradeManager* m = CCEUpgradeManager::sharedUpgradeManager();
	if(!m->isProcessing()) {
		CCEUpgradeManager::sharedUpgradeManager()->start();
	}
}
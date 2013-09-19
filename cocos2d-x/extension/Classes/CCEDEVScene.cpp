#include "CCEDEVScene.h"
#include "CCEAppUtil.h"

USING_NS_CC;

enum
{
    DEV_RESET_APP = 0,
	DEV_UPGRADE,
	DEV_UPGRADE_CONFIG,
    DEV_COUNT,    
};

const std::string g_DevMenuNames[DEV_COUNT] = {
    "Start Application",
	"Upgrade",
	"Upgrade Config",
};

const int g_DevMenu[] = {
	DEV_RESET_APP, DEV_UPGRADE, DEV_UPGRADE_CONFIG, -1
};

#define MY_FONT_SIZE			64
#define MY_LINE_SPACE          (MY_FONT_SIZE+8)

// layer
class CCEDEVSceneLayer : public CCLayer
{
public:
	virtual bool init(CCMenu* menu);  
	static CCEDEVSceneLayer* create(CCMenu* menu);

    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

private:
	CCPoint m_tBeginPos;
    CCMenu* m_pItemMenu;
};

static CCPoint s_tCurPos = CCPointZero;

bool CCEDEVSceneLayer::init(CCMenu* menu)
{
	if(!CCLayer::init()) {
		return false;
	}

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	m_pItemMenu = menu;    
    addChild(m_pItemMenu);

    setTouchEnabled(true);

    return true;
}

CCEDEVSceneLayer* CCEDEVSceneLayer::create(CCMenu* menu)
{
	CCEDEVSceneLayer* r = new CCEDEVSceneLayer();
	if(r->init(menu)) {
		r->autorelease();
		return r;
	}
	CC_SAFE_DELETE(r);
	return NULL;
}

void CCEDEVSceneLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator it = pTouches->begin();
    CCTouch* touch = (CCTouch*)(*it);

    m_tBeginPos = touch->getLocation();    
}

void CCEDEVSceneLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator it = pTouches->begin();
    CCTouch* touch = (CCTouch*)(*it);

    CCPoint touchLocation = touch->getLocation();    
    float nMoveY = touchLocation.y - m_tBeginPos.y;

    CCPoint curPos  = m_pItemMenu->getPosition();
    CCPoint nextPos = ccp(curPos.x, curPos.y + nMoveY);

    if (nextPos.y < 0.0f)
    {
        m_pItemMenu->setPosition(CCPointZero);
        return;
    }

	float ny = ((DEV_COUNT + 1)* MY_LINE_SPACE - CCEAppUtil::getVisibleRect().size.height);
	if(ny<0.0f)ny=0.0f;
    if (nextPos.y > ny)
    {
        m_pItemMenu->setPosition(ccp(0, ny));
        return;
    }

    m_pItemMenu->setPosition(nextPos);
    m_tBeginPos = touchLocation;
    s_tCurPos   = nextPos;
}

// CCEDEVScene
CCEDEVScene::CCEDEVScene()
{

}

CCEDEVScene::~CCEDEVScene()
{

}

CCEDEVScene* CCEDEVScene::create(fnAppStartup fn, void* data)
{
	CCEDEVScene* r = new CCEDEVScene();
	if(r->init(fn, data)) {

		CCMenu* menu = CCMenu::create();
		int i = 0;
		while(g_DevMenu[i]!=-1)
		{
			int id = g_DevMenu[i];
			CCLabelTTF* label = CCLabelTTF::create(g_DevMenuNames[id].c_str(), "Arial", MY_FONT_SIZE);
			CCMenuItemLabel* pMenuItem = CCMenuItemLabel::create(label, r, menu_selector(CCEDEVScene::menuCallback));
			menu->addChild(pMenuItem, 0, id);
			pMenuItem->setPosition( ccp( CCEAppUtil::getVisibleCenter().x, (CCEAppUtil::getVisibleTop().y - (i + 1) * MY_LINE_SPACE) ));
			i++;
		}

		menu->setContentSize(CCSizeMake(CCEAppUtil::getVisibleRect().size.width, (DEV_COUNT + 1) * (MY_LINE_SPACE)));
		menu->setPosition(s_tCurPos);

		CCEDEVSceneLayer *layer = CCEDEVSceneLayer::create(menu);
		if(layer!=NULL) {
			// add layer as a child to scene
			r->addChild(layer);
			r->autorelease();
			return r;
		}
	}
	CC_SAFE_DELETE(r);
	return NULL;
}

#include "CCEDEVUpgradeScene.h"
#include "CCEDEVUpgradeConfigScene.h"

void CCEDEVScene::menuCallback(CCObject * pSender)
{
    // get the userdata, it's the index of the menu item clicked
    CCMenuItem* pMenuItem = (CCMenuItem *)(pSender);
	int nIdx = pMenuItem->getTag();

	switch(nIdx) {
	case DEV_RESET_APP:
		startupApp();
		break;
	case DEV_UPGRADE: {
			CCEDEVUpgradeScene* s1 = CCEDEVUpgradeScene::create();
			CCDirector::sharedDirector()->pushScene(s1);
			break;
		}
	case DEV_UPGRADE_CONFIG: {
			CCEDEVUpgradeConfigScene* s2 = CCEDEVUpgradeConfigScene::create();
			CCDirector::sharedDirector()->pushScene(s2);
			break;
		}
	}
}
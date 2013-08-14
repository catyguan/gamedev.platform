#include "CCEScene.h"

CCEScene::CCEScene()
{
	
}

CCEScene::~CCEScene()
{
    
}

bool CCEScene::init()
{
	if(!CCScene::init()) {
		return false;
	}
	return true;
}

CCEScene* CCEScene::create()
{    
    CCEScene *pRet = new CCEScene();
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

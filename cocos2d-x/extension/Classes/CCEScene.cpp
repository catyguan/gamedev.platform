#include "CCEScene.h"
#include "../cocoa/CCValueSupport.h"

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

CC_BEGIN_CALLS(CCEScene, CCScene)
	CC_DEFINE_CALL(CCEScene, nextFrame)
CC_END_CALLS(CCEScene, CCScene)

CCValue CCEScene::CALLNAME(nextFrame)(CCValueArray& params) {
	CCValue call = ccvp(params,0);
	if(call.canCall()) {
		schedule(NULL, call, 0, 0, 0);
	}
	return CCValue::nullValue();
}
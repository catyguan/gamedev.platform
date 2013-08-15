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

void CCEScene::update4nextFrame(float d)
{
	CCValueArray tmp;
	for(size_t i=0;i<m_nextFrameCall.size();i++) {
		tmp.push_back(m_nextFrameCall[i]);
		tmp[i].retain();
	}
	m_nextFrameCall.clear();
	for(size_t i=0;i<tmp.size();i++) {
		CCValueArray ps;
		tmp[i].call(ps, false);
	}
	tmp.clear();
}

CC_BEGIN_CALLS(CCEScene, CCScene)
	CC_DEFINE_CALL(CCEScene, nextFrame)
CC_END_CALLS(CCEScene, CCScene)

CCValue CCEScene::CALLNAME(nextFrame)(CCValueArray& params) {
	CCValue call = ccvp(params,0);
	if(call.canCall()) {
		if(m_nextFrameCall.size()==0) {
			scheduleOnce(schedule_selector(CCEScene::update4nextFrame), 0);
		}
		m_nextFrameCall.push_back(call);
		m_nextFrameCall[m_nextFrameCall.size()-1].retain();
	}
	return CCValue::nullValue();
}
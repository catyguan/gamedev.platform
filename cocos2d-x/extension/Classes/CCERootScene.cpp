#include "CCERootScene.h"

USING_NS_CC;

CCERootScene::CCERootScene()
	: m_fnAppStartup(NULL)
	, m_appStartupData(NULL)
{

}

CCERootScene::~CCERootScene()
{

}

bool CCERootScene::init(fnAppStartup fn, void* data)
{	
	if(!CCEScene::init()) {    
        return false;
    }

	m_fnAppStartup = fn;
	m_appStartupData = data;

    return true;
}
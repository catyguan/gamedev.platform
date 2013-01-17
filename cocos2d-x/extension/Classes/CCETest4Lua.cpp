#include "CCETest4Lua.h"
#include "CCELuaApplication.h"

//
//CCETest4LuaLayer
//
CCETest4LuaLayer::CCETest4LuaLayer()
{
	
}

CCETest4LuaLayer::~CCETest4LuaLayer()
{
	
}

CCETest4LuaLayer* CCETest4LuaLayer::create(const char* packageName)
{    
    CCETest4LuaLayer *pRet = new CCETest4LuaLayer();
	if (pRet && pRet->init(packageName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }    
    return pRet;
}

CCETest4LuaLayer* CCETest4LuaLayer::create(CCValueArray& ps)
{
	CCValue name;
	if(ps.size()>0) {
		name = ps[0];
	}
	return create(name.stringValue().c_str());
}

bool CCETest4LuaLayer::init(const char* packageName)
{
    if (!CCELayerTouch::init())
    {
		return false;
	}
	
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	m_packageName = packageName;
	CCLayerColor* main = CCLayerColor::create(ccc4(128,128,128,255));
	this->addChild(main);

	CCELayerTouchItem* item = createTouch(main);
	item->onClick(this, touch_selector(CCETest4LuaLayer::clickHandler));

	CCLabelTTF* label = CCLabelTTF::create(packageName, "Arial", 24*2);
	label->setPosition(ccp(size.width/2,size.height/2));
	main->addChild(label);
    
	return true;
}

void CCETest4LuaLayer::clickHandler(CCNode* node, int type, CCTouch*)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(m_packageName));
	CCELuaApplication::sharedLuaHost()->pcall("include",ps);
}
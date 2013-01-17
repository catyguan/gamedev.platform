#ifndef __CCE_TEST4LUA_H_
#define __CCE_TEST4LUA_H_

#include "cocos2d.h"
#include "CCELayerTouch.h"
#include <string>

USING_NS_CC;

class CCETest4LuaLayer : public CCELayerTouch
{    
public:
    CCETest4LuaLayer();
    virtual ~CCETest4LuaLayer();

    /** creates an empty CCMenu */
	static CCETest4LuaLayer* create(const char* packageName);
    static CCETest4LuaLayer* create(CCValueArray& ps);

    virtual bool init(const char* packageName);

	void clickHandler(CCNode* node, int type, CCTouch*);

protected:
	std::string m_packageName;
};

#endif

#ifndef  __CCE_SF_LUA_LOADER_H__
#define  __CCE_SF_LUA_LOADER_H__

#include "cocos2d.h"

USING_NS_CC;

class CCELuaApplication;
class CCEFSLuaLoader
{
public:
	CCEFSLuaLoader();
	virtual ~CCEFSLuaLoader(void);

public:
	void bind(CCELuaApplication* app);	

	static std::string CALLBACK loader_callback(CCELuaApplication* host, void* data, const char* name);

};

#endif  // __CCE_DIRECTOR_H__
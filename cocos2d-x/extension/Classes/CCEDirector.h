#ifndef  __CCE_DIRECTOR_H__
#define  __CCE_DIRECTOR_H__

#include "CCELuaHost.h"

class CCEDirector
{
public:
	CCEDirector();
	virtual ~CCEDirector();

public:


	void install(CCELuaHost* host);
	static CCEDirector* sharedDirector(void);
    static void purgeSharedDirector(void);    

protected:
	// luaCall
	static bool CALLBACK popScene(CCELuaHost* host, void* data, int callId, CCELuaValueArray& params);

	virtual void initHost(CCELuaHost* host);
};

#endif
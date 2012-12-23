#ifndef  __CCE_LUAHOST_H__
#define  __CCE_LUAHOST_H__

typedef struct lua_State lua_State;

#include "cocos2d.h"
#include "luadef.h"

class CCELuaHost;
typedef bool (CALLBACK *CCELuaCall)(CCELuaHost* host, void* data, int callId, CCELuaValueArray& params);
typedef std::string (CALLBACK *CCELuaLoader)(CCELuaHost* host, void* data, const char* name);

typedef struct {
	CCELuaCall call;
	void* data;
} CCELuaCallItem;

typedef struct {
	int id;
	int time;
	int delay;
	int fix;
} CCELuaHostTimer;

class CCELuaHost
{
public:
    CCELuaHost();
    virtual ~CCELuaHost();

private:
	bool handleCallback(lua_State* L, CCELuaValueArray& data);

public:    
    static void setInstance(CCELuaHost* host);

	void open();
	bool isOpen() {
		return state!=NULL;
	}
	void close();

	void addpath(const char* path);
	void setvar(const char* key, const char* value);
	bool pcall(const char* fun, CCELuaValueArray& data);
	std::string eval(const char* content);

	void setLoader(CCELuaLoader loader,void* data);
	void setCall(const char* name, CCELuaCall call, void* data);	

	bool callResponse(int callId, const char* err, CCELuaValueArray& params);

	void enablePrintLog();
	void require(const char* package);

	void createApp(const char* appType, 
		std::list<std::string> pathList, std::list<std::string> bootstrapList);
	
	int luaCallback(lua_State* L);
	static bool luaCallError(CCELuaValueArray& r,  const char * format, ...);
	int luaLoad(lua_State* L);
        
    static CCELuaHost* sharedLuaHost(void);
    static void purgeSharedLuaHost(void);    

	static void CALLBACK appRunnable(void* data, long mstick);

private:	
    lua_State* state;
	CCELuaLoader loader;
	void* loaderData;
	
	std::map<std::string, CCELuaCallItem> calls;

	std::list<CCELuaHostTimer> timers;
	int minWaitTime;
	int nowTick;
	long startTick;	
};

#endif // __CCE_LUAHOST_H__


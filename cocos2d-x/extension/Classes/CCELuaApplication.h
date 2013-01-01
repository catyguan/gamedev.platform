#ifndef  __CCE_LUAHOST_H__
#define  __CCE_LUAHOST_H__

#include "CCEApplication.h"
#include <list>

typedef struct lua_State lua_State;

class CCELuaApplication;
typedef bool (CALLBACK *CCELuaCall)(CCELuaApplication* host, void* data, int callId, CCValueArray& ctx);
typedef std::string (CALLBACK *CCELuaLoader)(CCELuaApplication* host, void* data, const char* name);

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

class CCELuaApplication : public CCEApplication
{
public:
    CCELuaApplication();
    virtual ~CCELuaApplication();

private:
	bool handleCallback(lua_State* L, CCValueArray& data);

public:    
	void open();
	bool isOpen() {
		return state!=NULL;
	}
	void close();

	void addpath(const char* path);
	void setvar(const char* key, const char* value);
	bool pcall(const char* fun, CCValueArray& data);
	std::string eval(const char* content);

	void setLoader(CCELuaLoader loader,void* data);
	void setCall(const char* name, CCELuaCall call, void* data);	

	bool callResponse(int callId, const char* err, CCValueArray& params);

	void enablePrintLog();
	void require(const char* package);

	void createApp(const char* appType, 
		std::list<std::string> pathList, std::list<std::string> bootstrapList);
	
	int luaCallback(lua_State* L);
	static bool luaCallError(CCValueArray& r,  const char * format, ...);
	int luaLoad(lua_State* L);
        
    static CCELuaApplication* sharedLuaHost(void);

	static void CALLBACK appRunnable(void* data, long mstick);

private:	
    lua_State* state;
	CCELuaLoader loader;
	void* loaderData;
	
	std::map<std::string, CCELuaCallItem> calls;

	std::list<CCELuaHostTimer> timers;
	int minWaitTime;
	int nowTick;
	int gcTick;
	int startTick;	
};

class CCELuaCallResponse : public CCObject
{
public:
	~CCELuaCallResponse();

public:
	static CCELuaCallResponse* create(CCELuaApplication* app, int cid);

	virtual CCValue invoke(CCValueArray& params);

protected:
	CCELuaCallResponse();

	CCELuaApplication* m_App;
	int m_callId;
};

#endif // __CCE_LUAHOST_H__


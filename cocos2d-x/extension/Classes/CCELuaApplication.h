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

class CCELuaHost;
class CCELuaApplication : public CCEApplication
{
public:
    CCELuaApplication();
    virtual ~CCELuaApplication();

public:    
	void open();
	bool isOpen();
	void close();

	void addpath(const char* path);
	void setvar(const char* key, const char* value);
	bool pcall(const char* fun, CCValueArray& data, CCValueArray& result);
	std::string eval(const char* content);

	void setLoader(CCELuaLoader loader,void* data);
	void setCall(const char* name, CCELuaCall call, void* data);	

	bool callResponse(int callId, const char* err, CCValueArray& params);

	void enablePrintLog();
	void require(const char* package);

	void createApp(const char* appType, 
		std::list<std::string> pathList, std::list<std::string> bootstrapList);
	
    static CCELuaApplication* sharedLuaHost(void);

	static void CALLBACK appRunnable(void* data, long mstick);

protected:
	virtual void cleanup();

private:	
    CCELuaHost* m_host;
	CCELuaLoader m_loader;
	void* m_loaderData;
	
	std::map<std::string, CCELuaCallItem> m_calls;

	std::list<CCELuaHostTimer> m_timers;
	int m_minWaitTime;
	int m_nowTick;
	int m_gcTick;
	int m_startTick;	

	friend class CCELuaHost;
};

class CCELuaResponseObject : public CCObject
{
public:
	~CCELuaResponseObject();

public:
	static CCELuaResponseObject* create(CCELuaApplication* app, int cid);
	static CCELuaResponseObject* create(CCELuaApplication* app, int cid, CCValueArray& ret);

	virtual CCValue invoke(CCValueArray& params);

protected:
	CCELuaResponseObject();

	CCELuaApplication* m_App;
	int m_callId;
	CCValueArray m_Return;
};

class CCELuaCallObject: public CCObject
{
public:
	~CCELuaCallObject();

public:
	static CCELuaCallObject* create(CCELuaApplication* app, const char* fun);
	static CCELuaCallObject* create(CCELuaApplication* app, const char* fun, CCValueArray& ret);

	virtual CCValue invoke(CCValueArray& params);

protected:
	CCELuaCallObject();

	CCELuaApplication* m_App;
	std::string m_csFun;
	CCValueArray m_Params;
};

class CCELuaClosureObject : public CCObject
{
public:
	~CCELuaClosureObject();

public:
	static CCELuaClosureObject* create(CCELuaApplication* app, int cid);

	virtual CCValue invoke(CCValueArray& params);

protected:
	CCELuaClosureObject();

	CCELuaApplication* m_App;
	int m_callId;
};

#endif // __CCE_LUAHOST_H__


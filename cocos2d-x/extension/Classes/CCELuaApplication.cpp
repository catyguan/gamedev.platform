#include "CCELuaApplication.h"

USING_NS_CC;

// CCELuaApplication
#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// JSON
void luamodule_cjson(lua_State *L);  
void luamodule_cjson_safe(lua_State *L);
// MD5
void luamodule_md5(lua_State *L);
// DES56
void luamodule_des56(lua_State *L);

// luaext
void addSearchPath(lua_State* L, const char* path);

int isLuaArray(lua_State *l, int idx);

void setModuleLoader(lua_State* L, lua_CFunction loader,int replace );

#ifdef __cplusplus
}
#endif

static CCELuaApplication* getHost(lua_State* L) {
	lua_pushstring( L , "_cocos2dhost");
	lua_rawget( L , LUA_REGISTRYINDEX );

	if( !lua_islightuserdata(L, -1))	
	{
		lua_pushstring( L , "invalid _cocos2d host object" );
		lua_error( L );
	}	
	CCELuaApplication* host = (CCELuaApplication*) lua_touserdata(L , -1);
	lua_pop(L,1);
	return host;
}

static int lua_hostloader(lua_State *L) {	
	CCELuaApplication* host = getHost(L);
	return host->luaLoad(L);
}

static int lua_hostcall(lua_State *L) {
	CCELuaApplication* host = getHost(L);
	return host->luaCallback(L);
}

static struct luaL_Reg cocos2dlib[] = {
  {"call", lua_hostcall},
  {NULL, NULL}
};


int luaopen_cocos2d_ext (lua_State *L) {
  luaL_openlib(L, "c2dx", cocos2dlib, 0);  
  return 1;
}

#define FLAG_OBJECT "_FObject"
#define FLAG_OMCALL "_FOMCall"
#define FLAG_FUNCTION "_FFunction"
#define FLAG_OCALL "_FOCall"

static int isLuaFlag( lua_State * L , int idx, const char* flag )
{
   if ( !lua_isuserdata( L , idx ) )
      return 0;

   if ( lua_getmetatable( L , idx ) == 0 )
      return 0;

   lua_pushstring( L , flag );
   lua_rawget( L , -2 );

   if (lua_isnil( L, -1 ))
   {
      lua_pop( L , 2 );
      return 0;
   }
   lua_pop( L , 2 );
   return 1;
}

int luaObjectIndex( lua_State * L );
int luaFunctionCall( lua_State * L );
int luaObjectCall( lua_State* L);

int luaMemGC( lua_State * L )
{
	if ( isLuaFlag( L , 1 , FLAG_OBJECT ) )
	{
		CCObject* obj = *((CCObject**) lua_touserdata( L , 1 ));
		if(obj!=NULL) {
			obj->release();
		}
		return 0;	
	}
	if ( isLuaFlag( L , 1 , FLAG_OMCALL ) ) {
		CCObject* obj = *((CCObject**) lua_touserdata( L , 1 ));
		if(obj!=NULL) {
			obj->release();
		}
		return 0;
	}
	if ( isLuaFlag( L , 1 , FLAG_OCALL ) ) {
		CCObject* obj = *((CCObject**) lua_touserdata( L , 1 ));
		if(obj!=NULL) {
			obj->release();
		}
		return 0;
	}
	CCLOG("unknow userdata");
	return 0;
}

static void pushCCObject(lua_State* L,CCObject* obj)
{
	if(obj==NULL) {
		lua_pushnil(L);
	}

	CCObject** userData = ( CCObject** ) lua_newuserdata( L , sizeof( CCObject* ) );
	*userData = obj;
	obj->retain();

	/* Creates metatable */
	lua_newtable( L );

	/* pushes the __index metamethod */
	lua_pushstring( L , "__index" );
	lua_pushcfunction( L , &luaObjectIndex );
	lua_rawset( L , -3 );

	/* pusher the __gc metamethod */
	lua_pushstring( L , "__gc");
	lua_pushcfunction( L , &luaMemGC );
	lua_rawset( L , -3 );

	lua_pushstring( L , FLAG_OBJECT);
	lua_pushboolean( L , 1 );
	lua_rawset( L , -3 );

	lua_setmetatable( L , -2 );
}

static void pushLuaValue(lua_State* L, const CCValue* v)
{
	if(v==NULL) {
		lua_pushnil(L);
		return;
	}
	CCValueType t = v->getType();
	if(t==CCValueTypeInt) {
		lua_pushinteger(L, v->intValue());	
		return;
	} else if(t==CCValueTypeString) {
		lua_pushstring(L, v->stringValue().c_str());
		return;
	} else if(t==CCValueTypeBoolean) {
		lua_pushboolean(L, v->booleanValue()?1:0);
		return;
	} else if(t==CCValueTypeNumber) {
		lua_pushnumber(L, v->numberValue());
		return;
	} else if(t==CCValueTypeArray) {
		const CCValueArray& o = v->arrayValue();
		lua_newtable(L);
		int i = 1;
		for (CCValueArrayIterator it = o.begin(); it != o.end(); ++ it)
		{	
			lua_pushnumber(L, i+1);
			pushLuaValue(L, &(*it));
			lua_settable(L,-3);
		}
		return;
	} else if(t==CCValueTypeMap) {
		const CCValueMap& o = v->mapValue();
		lua_newtable(L);
		for(CCValueMapIterator it = o.begin(); it != o.end(); it++)
		{
			lua_pushstring(L, it->first.c_str());
			pushLuaValue(L, &(it->second));
			lua_settable(L,-3);
		}
		return;
	} else if(t==CCValueTypeObject) {
		CCObject* obj = v->objectValue();
		pushCCObject(L, obj);
		return;
	} else if(t==CCValueTypeFunction) {
		CC_FUNCTION_CALL fn = v->fcallValue();
		CC_FUNCTION_CALL* userData = ( CC_FUNCTION_CALL* ) lua_newuserdata( L , sizeof( CC_FUNCTION_CALL ) );
		*userData = fn;

		/* Creates metatable */
		lua_newtable( L );

		/* pushes the __index metamethod */
		lua_pushstring( L , "__call" );
		lua_pushcfunction( L , &luaFunctionCall );
		lua_rawset( L , -3 );

		lua_pushstring( L , FLAG_FUNCTION);
		lua_pushboolean( L , 1 );
		lua_rawset( L , -3 );

		lua_setmetatable( L , -2 );
		return;
	} else if(t==CCValueTypeObjectCall) {
		const CCOCall* call = v->ocallValue();
		CCOCall* userData = ( CCOCall* ) lua_newuserdata( L , sizeof( CCOCall ) );
		userData->pObject = call->pObject;
		userData->call = call->call;
		call->pObject->retain();

		/* Creates metatable */
		lua_newtable( L );

		/* pushes the __index metamethod */
		lua_pushstring( L , "__call" );
		lua_pushcfunction( L , &luaObjectCall );
		lua_rawset( L , -3 );

		lua_pushstring( L , "__gc" );
		lua_pushcfunction( L , &luaMemGC);
		lua_rawset( L , -3 );

		lua_pushstring( L , FLAG_OCALL);
		lua_pushboolean( L , 1 );
		lua_rawset( L , -3 );

		lua_setmetatable( L , -2 );
		return;
	}
	lua_pushnil(L);	
}

static int pushStackData(lua_State* L, CCValueArray& params) {
	int i = 0;	 
	for (CCValueArrayIterator it = params.begin(); it != params.end(); ++ it)
	{	
		i++;
		pushLuaValue(L, &(*it));
	}
	return i;
}

static CCValue popLuaValue(lua_State* L, int idx) {
	int type = lua_type(L,idx);		
	switch(type) {
		case LUA_TBOOLEAN: {
			return CCValue::booleanValue(lua_toboolean(L,idx)!=0);
		}
		case LUA_TNUMBER: {
			lua_Integer v1 = lua_tointeger(L,idx);
			lua_Number v2 = lua_tonumber(L,idx);
			if(v1==v2) {
				return CCValue::intValue((int) v1);
			} else {
				return CCValue::numberValue((double) v2);
			}				
		}
		case LUA_TSTRING: {
			const char* v = lua_tostring(L, idx);			
			return CCValue::stringValue(v);
		}		
		case LUA_TTABLE: {				
			if(idx<0) {
				idx = lua_gettop(L)+idx+1;
			}
			int len = isLuaArray(L, idx);
			if (len > 0) {
				CCValueArray r;
				for (int i = 1; i <= len; i++) {		
					lua_rawgeti(L, idx, i);
					r.push_back(popLuaValue(L, -1));
					lua_pop(L, 1);
				}
				return CCValue::arrayValue(r);
			} else {
				CCValueMap r;
				lua_pushnil(L);
				while (lua_next(L, idx) != 0) {
					const char* key = lua_tostring(L, -2);
					if(key!=NULL) {					
						std::string s = key;
						r[s] = popLuaValue(L, -1);						
					}
					lua_pop(L, 1);
				}
				return CCValue::mapValue(r);
			}
		}
		case LUA_TUSERDATA: {
			if(isLuaFlag(L, idx, FLAG_OBJECT)) {
				CCObject* obj = *((CCObject**) lua_touserdata( L , 1 ));
				return CCValue::objectValue(obj);
			}
			if(isLuaFlag(L, idx, FLAG_FUNCTION)) {
				CC_FUNCTION_CALL fn = *((CC_FUNCTION_CALL*) lua_touserdata( L , 1 ));
				return CCValue::fcallValue(fn);
			}
			if(isLuaFlag(L, idx, FLAG_OCALL)) {
				CCOCall* call = (CCOCall*) lua_touserdata( L , 1 );
				return CCValue::ocallValue(call->pObject, call->call);
			}
		}
		case LUA_TNIL:
		default:		
			return CCValue::nullValue();
	}
}

static void popStackData(lua_State* L, CCValueArray& params,int top,int nresults) {
	for(int i=1;i<=nresults;i++) {		
		params.push_back(popLuaValue(L,top+i));
	}
	if(nresults>0) {
		lua_pop(L,nresults);
	}	
}

int luaObjectMethodCall( lua_State * L )
{
	if ( !isLuaFlag( L , 1 , FLAG_OMCALL ) )
	{
		lua_pushstring( L , "not CCObject call" );
		lua_error( L );
	}
	CCObject** ptr = (CCObject**) lua_touserdata( L , 1 );
	CCObject* obj = *(ptr);
	char* p = (char*) (ptr+1);

	int pc = lua_gettop(L)-1;
	CCValueArray params;
	popStackData(L, params, 1, pc);

	try {
		CCValue r = obj->call(p, params);
		pushLuaValue(L, &r);
	} catch(std::string err) {
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}
	return 1;
}

int luaFunctionCall( lua_State * L )
{
	if ( !isLuaFlag( L , 1 , FLAG_FUNCTION) )
	{
		lua_pushstring( L , "not Function call" );
		lua_error( L );
	}
	CC_FUNCTION_CALL* ptr = (CC_FUNCTION_CALL*) lua_touserdata( L , 1 );
	CC_FUNCTION_CALL fn = *(ptr);
	
	int pc = lua_gettop(L)-1;
	CCValueArray params;
	popStackData(L, params, 1, pc);

	try {
		CCValue r = fn(params);
		pushLuaValue(L, &r);
	} catch(std::string err) {
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}
	return 1;
}

int luaObjectCall( lua_State * L )
{
	if ( !isLuaFlag( L , 1 , FLAG_OCALL) )
	{
		lua_pushstring( L , "not Object call" );
		lua_error( L );
	}
	CCOCall* ptr = (CCOCall*) lua_touserdata( L , 1 );
		
	int pc = lua_gettop(L)-1;
	CCValueArray params;
	popStackData(L, params, 1, pc);

	try {
		CCValue r = (ptr->pObject->*ptr->call)(params);
		pushLuaValue(L, &r);
	} catch(std::string err) {
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}
	return 1;
}

int luaObjectIndex( lua_State * L )
{
	if ( !isLuaFlag( L , 1 , FLAG_OBJECT ) )
	{
		lua_pushstring( L , "not CCObject" );
		lua_error( L );
	}
	CCObject* obj = *((CCObject**) lua_touserdata( L , 1 ));
	const char* key = lua_tostring(L, 2);
	if(obj->canCall(key)) {
		CCObject** userData = ( CCObject** ) lua_newuserdata( L , sizeof( CCObject* )+strlen(key)+1 );
		*userData = obj;
		obj->retain();
		char* p = (char*) (userData+1);
		strcpy(p, key);

		/* Creates metatable */
		lua_newtable( L );

		/* pushes the __call metamethod */
		lua_pushstring( L , "__call" );
		lua_pushcfunction( L , &luaObjectMethodCall );
		lua_rawset( L , -3 );

		/* pusher the __gc metamethod */
		lua_pushstring( L , "__gc");
		lua_pushcfunction( L , &luaMemGC );
		lua_rawset( L , -3 );

		lua_pushstring( L , FLAG_OMCALL);
		lua_pushboolean( L , 1 );
		lua_rawset( L , -3 );

		lua_setmetatable( L , -2 );
		return 1;
	}
	CCObject* sobj = obj->findChildById(key);
	pushCCObject(L, sobj);
	return 1;
}

CCELuaApplication::CCELuaApplication()
{
	state = NULL;
	loader = NULL;
	loaderData = NULL;

	minWaitTime = -1;
	startTick = -1;
	nowTick = 0;
	gcTick = 0;
}

CCELuaApplication::~CCELuaApplication() 
{
	if(isOpen()) {
		close();
	}
}

void CCELuaApplication::open()
{
    if (isOpen()) {
		return;
	}
    // luaState.open();
	lua_State* L = ::luaL_newstate();
	::luaL_openlibs(L);  /* open libraries */
	::luamodule_cjson(L);
	::luamodule_cjson_safe(L);
	::luamodule_md5(L);
	::luamodule_des56(L);
	::luaopen_cocos2d_ext(L);

	lua_pushstring( L , "_cocos2dhost");
	lua_pushlightuserdata(L, this);
	lua_settable( L , LUA_REGISTRYINDEX );		
	state = L;
}

void CCELuaApplication::close()
{
	if (isOpen())
    {
		lua_State* L = state;
		if(L!=NULL) {
			::lua_close(L);			
		}
        state = NULL;		
	}
	calls.clear();
}

void CCELuaApplication::addpath(const char* path)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(path!=NULL);
	::addSearchPath( state , path);
}

void CCELuaApplication::setvar(const char* key, const char* value)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(key!=NULL);
	CC_ASSERT(value!=NULL);

	::lua_pushstring(state, value);
	::lua_setfield(state, LUA_GLOBALSINDEX, key);
}

bool CCELuaApplication::pcall(const char* fun, CCValueArray& data)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(fun!=NULL);

	lua_State* L = state;
	int top = lua_gettop(L);

	/* function to be called */
	lua_getfield(L, LUA_GLOBALSINDEX, fun);
	int sz = pushStackData(L, data);
	int err = lua_pcall(L, sz, LUA_MULTRET, 0);
	sz = lua_gettop(L) - top;
	data.clear();
	popStackData(L, data, top, sz);
	if(err==0) {
		return true;
	}
	if(data.size()>0 && data[0].isString()) {
		CCLOG("[LuaHost pcall] call '%s' fail, error - %s", fun, data[0].stringValue().c_str());
	} else {
		CCLOG("[LuaHost pcall] call '%s' fail, unknow error", fun);
	}
	return false;
}

std::string CCELuaApplication::eval(const char* content)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(content!=NULL);

	lua_State* L = state;		
	int err = luaL_dostring(L, content);
	if(err!=0) {
		const char* s = lua_tostring(L,-1);
		lua_pop(L,1);
		return std::string(s);
	}
	return CCValue::EMPTY;
}

void CCELuaApplication::setLoader(CCELuaLoader loader, void* data)
{
	CC_ASSERT(isOpen());

	this->loader = loader;
	this->loaderData = data;

	setModuleLoader(state, lua_hostloader, 1);
}

int CCELuaApplication::luaLoad(lua_State* L)
{
	if(loader==NULL) {
		lua_pushstring(L, "loader is null");
		return 1;
	}
	const char* lname = lua_tostring(L,-1);
	try{
		std::string content = loader(this, loaderData, lname);
		luaL_loadbuffer(L, content.c_str(), content.length(), lname);				
		return 1;				
	} catch(std::string err){
		lua_pushstring(L, err.c_str());
		return 1;
	}
}

void CCELuaApplication::setCall(const char* name, CCELuaCall call, void* data)
{
	CC_ASSERT(name);
	CCELuaCallItem c;
	c.call = call;
	c.data = data;
	calls[name] = c;
}

bool CCELuaApplication::luaCallError(CCValueArray& r, const char * format, ...)
{
	char buf[1024];
	va_list arglist;
	va_start(arglist, format);
	_vsnprintf(buf, 1024, format, arglist);
	va_end(arglist);
	
	r.clear();
	r.push_back(CCValue::stringValue(buf));
    return false;
}

bool CCELuaApplication::handleCallback(lua_State* L, CCValueArray& data)
{
	if(data.size()==0) {
		return luaCallError(data, "invalid call type");
	}
	std::string name = data[0].stringValue();
	data.erase(data.begin());

	if (name.compare("hostCall")==0) {
		if(data.size()<2) {
			return luaCallError(data, "invalid callId & methodId");
		}
		int cid = data[0].intValue();
		data.erase(data.begin());

		std::string method = data[0].stringValue();		
		data.erase(data.begin());

		std::map<std::string, CCELuaCallItem>::const_iterator it = calls.find(method);
		if(it==calls.end()) {
			return luaCallError(data, "invalid methodId '%s'", method.c_str()); 
		}		
		bool r = false;
		try {
			r = it->second.call(this, it->second.data, cid, data);
		} catch (std::string e) {
			CCLOG("[Host::handleCallback EXCEPTION] %s", e.c_str());
			return luaCallError(data, e.c_str());
		}
		if (r) {
			data.insert(data.begin(), CCValue::booleanValue(true));
		} else {
			data.clear();
			if(cid<=0) {
				return luaCallError(data, "invalid syn call '%s[%d]'", method.c_str(), cid);
			}
			data.push_back(CCValue::booleanValue(false));
		}
		return true;
	} else if (name.compare("hostTimer")==0) {
        if (data.size() < 3)
        {
            return luaCallError(data, "invalid timerId,fix,delay");
        }
		int tid = data[0].intValue();
		if(tid<=0) {
			return luaCallError(data, "invalid timerId '%d'", tid);
		}
		if (tid > 0) {                    
			int fix = data[1].intValue();
			int delay = data[2].intValue();
			if (delay <= 0) {
				return luaCallError(data, "invalid timer delay");
			}
            CCLOG("setTimer - %d,%d,%d", tid, fix, delay);
			CCELuaHostTimer ntimer;
			ntimer.id = tid;
			ntimer.delay = delay;
			ntimer.fix = fix;
			ntimer.time = nowTick+delay;
			timers.push_back(ntimer);
			if(minWaitTime<0 || ntimer.time<minWaitTime) {
				minWaitTime = ntimer.time;
			}
			data.clear();
			return true;
		}
	} else if (name.compare("print")==0) {
		if(data.size()>0) {
			std::string msg = data[0].stringValue();
			CCLOG("[LUA print] %s", msg.c_str());
		}
		data.clear();
        return true;
	}
	std::string s = "invalid callType '";
	s += name;
	s += "'";
	return luaCallError(data, s.c_str()); 
}

int CCELuaApplication::luaCallback(lua_State* L)
{
	CCValueArray data;
	popStackData(L, data,0,lua_gettop(L));
	try {
		bool done = handleCallback(L, data);
		int r = pushStackData(L, data);
		if(done)return r;
		CCLOG("[Host::luaCallback ERROR] %s", lua_tostring(state, -1));
		return lua_error(L);
	} catch(std::string err){		
		CCLOG("[Host::luaCallback EXCEPTION] %s", err.c_str());
		return luaL_error(L,err.c_str());
	}	
}

CCELuaApplication* CCELuaApplication::sharedLuaHost(void)
{
	return (CCELuaApplication*) sharedApp();
}

bool CCELuaApplication::callResponse(int callId, const char* err, CCValueArray& data)
{	
	if(err==NULL) {
		data.insert(data.begin(), CCValue::nullValue());
	} else {
		data.insert(data.begin(), CCValue::stringValue(err));
	}
	data.insert(data.begin(), CCValue::intValue(callId));
	return pcall("luaCallResponse", data);
}

void CCELuaApplication::enablePrintLog()
{
	CC_ASSERT(isOpen());
	std::string err = eval("hostOrgPrint = print\n print = function(...)\n local msg = \"\"\n for i=1, arg.n do msg = msg .. tostring(arg[i])..\"\\t\" end\n c2dx.call(\"print\", msg)\n end\n");
	err.c_str();
}

void CCELuaApplication::appRunnable(void* data, long mstick)
{
	CC_ASSERT(data);
	CCELuaApplication* host = (CCELuaApplication*) data;
	CC_ASSERT(host->isOpen());

	if(host->startTick==-1) {
		host->startTick = mstick;
	}
	host->nowTick = mstick - host->startTick;

	// CCLOG("[LuaHost] apprun %d", host->nowTick);
	if(host->minWaitTime>=0 && host->minWaitTime<host->nowTick) {
		int mintm = host->nowTick+3600*1000;
		for (std::list<CCELuaHostTimer>::const_iterator it = host->timers.begin(); it != host->timers.end(); )
		{	
			std::list<CCELuaHostTimer>::const_iterator cit = it;			

			if(cit->time <= host->nowTick) {
				CCValueArray ps;
				ps.push_back(CCValue::intValue(cit->id));
				ps.push_back(CCValue::intValue(cit->fix));
				bool r = host->pcall("luaTimerResponse", ps);
				if(r) {
					CCLOG("[LuaHost] timer[%d] => done - %s", cit->id, (ps.size()>0?ps[0].booleanValue():false)?"true":"false");
				} else {
					CCLOG("[LuaHost] timer[%d] => fail - %s", cit->id, (ps.size()>0?ps[0].stringValue():CCValue::EMPTY).c_str());
				}
                if (r && cit->fix > 0)
                {
					if(ps.size()>0 && ps[0].booleanValue()) {
						CCELuaHostTimer ntimer;
						ntimer.id = cit->id;
						ntimer.delay = cit->delay;
						ntimer.fix = cit->fix;
						ntimer.time = cit->time+cit->fix;
						host->timers.push_back(ntimer);
					} else {
						CCLOG("[LuaHost] timer[%d] => stop fix timer", cit->id);
					}
                }
				++ it;
				host->timers.erase(cit);
			} else {
				if(cit->time < mintm) {
					mintm = cit->time;
				}
				++ it;
			}			
		}
		host->minWaitTime = mintm;
	}	
	lua_gc(host->state,LUA_GCSTEP,1);
}

void CCELuaApplication::require(const char* package)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(package));
	pcall("require", ps);
}

void CCELuaApplication::createApp(const char* appType, 
		std::list<std::string> pathList, std::list<std::string> bootstrapList)
{
	setvar("LUA_APP_TYPE",appType);	

	for(std::list<std::string>::const_iterator it = pathList.begin(); it != pathList.end(); it++) {
		addpath(it->c_str());			
	}
	for(std::list<std::string>::const_iterator it = bootstrapList.begin(); it != bootstrapList.end(); it++) {
		require(it->c_str());
	}	
}
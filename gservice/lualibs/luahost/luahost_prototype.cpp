#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>

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

#define LUA_TINT	-1
#define LUA_TARRAY	-2
#define LUA_TMAP	-3

#define LUA_FUNCTION_HOST_RESPONSE				"_API_host_response"
#define LUA_FUNCTION_HOST_CLOSURE_ADD			"_API_host_closure_add"
#define LUA_FUNCTION_HOST_CLOSURE_CALL			"_API_host_closure_call"
#define LUA_FUNCTION_HOST_CLOSURE_REMOVE		"_API_host_closure_remove"
#define LUA_FUNCTION_HOST_TIMER_RESPONSE		"_API_host_timer_response"

/*
typedef void* LuaHostValue;
typedef void* LuaHostValue_Ref;
typedef void* LuaHostArray;
typedef void* LuaHostArray_Ref;
typedef void* LuaHostArrayIterator;
typedef void* LuaHostMap;
typedef void* LuaHostMap_Ref;
typedef void* LuaHostMapIterator;
*/
class LuaHostPrototype;
class LuaHostValuePrototype
{
public:
	virtual int getLuaType(LuaHostValue_Ref value) = 0;
	
	virtual LuaHostArray newArray() = 0;
	virtual int arraySize(LuaHostArray_Ref array) = 0;
	virtual LuaHostArrayIterator arrayBegin(LuaHostArray_Ref array) = 0;
	virtual bool arrayValid(LuaHostArray_Ref array, LuaHostArrayIterator& it) =0;
	virtual bool arrayNext(LuaHostArray_Ref array, LuaHostArrayIterator& it) = 0;
	virtual LuaHostValue_Ref arrayGetAt(LuaHostArray_Ref array, LuaHostArrayIterator it) = 0;
	virtual LuaHostValue_Ref arrayGetAt(LuaHostArray_Ref array, int idx) = 0;
	virtual void arrayAdd(LuaHostArray_Ref array, LuaHostValue_Ref v) = 0;
	virtual LuaHostValue arrayPopFirst(LuaHostArray_Ref array) = 0;
	virtual void arrayPushFirst(LuaHostArray_Ref array,LuaHostValue_Ref v) = 0;
	virtual void resetArray(LuaHostArray_Ref array) = 0;	
	virtual void releaseArray(LuaHostArray_Ref v) = 0;
	
	virtual LuaHostMap newMap() = 0;
	virtual LuaHostMapIterator mapBegin(LuaHostMap_Ref map) = 0;
	virtual bool mapValid(LuaHostMap_Ref map, LuaHostMapIterator& it) = 0;
	virtual bool mapNext(LuaHostMap_Ref map, LuaHostMapIterator& it) = 0;
	virtual std::string mapGetKey(LuaHostMap_Ref map, LuaHostMapIterator it) = 0;
	virtual LuaHostValue_Ref mapGetValue(LuaHostMap_Ref map, LuaHostMapIterator it) = 0;
	virtual void mapSet(LuaHostMap_Ref map, const char* key, LuaHostValue_Ref v) = 0;
	virtual void releaseMap(LuaHostMap_Ref map) = 0;
	
	virtual int intValue(LuaHostValue_Ref v) = 0;
	virtual std::string stringValue(LuaHostValue_Ref v) = 0;
	virtual bool booleanValue(LuaHostValue_Ref v) = 0;
	virtual double numberValue(LuaHostValue_Ref v) = 0;
	virtual LuaHostArray_Ref arrayValue(LuaHostValue_Ref v) = 0;
	virtual LuaHostMap_Ref mapValue(LuaHostValue_Ref v) = 0;
	
	virtual LuaHostValue newBoolean(bool v) = 0;
	virtual LuaHostValue newInt(int v) = 0;
	virtual LuaHostValue newNumber(double v) = 0;
	virtual LuaHostValue newString(const char* v) = 0;
	virtual LuaHostValue newArrayValue(LuaHostArray_Ref v) = 0;
	virtual LuaHostValue newMapValue(LuaHostMap_Ref v) = 0;
	virtual LuaHostValue newNull() = 0;
	
	virtual void pushUserdata(lua_State* L, LuaHostValue_Ref v) = 0;
	virtual LuaHostValue popUserdata(lua_State* L, const char* flag, const void* data) = 0;
};

class LuaHostPrototype
{
public:
	LuaHostPrototype();
	virtual ~LuaHostPrototype();

	virtual void open();
	bool isOpen(){return m_state!=NULL;};
	virtual void openHostLibs();
	virtual void close();
	
	void addpath(const char* path);
	void setvar(const char* key, const char* value);
	bool buildCallError(LuaHostArray_Ref r, const char * format, ...);
	
	virtual bool pcall(const char* fun, LuaHostArray_Ref params);
	std::string eval(const char* content);
	virtual bool reponseLuaAInvoke(int callId, const char* err, LuaHostArray_Ref data);	
	
public:
	virtual int lapi_load() = 0;
	virtual int lapi_call();
	virtual int lapi_closure();
	virtual LuaHostValue lapi_createClosure(int callId) = 0;
	virtual std::string lapi_tostring(const char* flag, void* userdata) = 0;
	virtual void lapi_ud_memgc(const char* flag, void* userdata) = 0;
	virtual int lapi_ud_index(const char* flag, void* userdata, const char* key) = 0;
	virtual int lapi_ud_call();	

	void gc(int v) {
		lua_gc(m_state,LUA_GCSTEP,v);
	};
	void enableModuleLoader();
	void pushUserdata(const char* flag, void* userdata,int size, bool canIndex, bool canCall);
	void pushValue(LuaHostValue v);
		
protected:
	virtual LuaHostValuePrototype* valuePrototype() = 0;	
	virtual int lapi_userdataCall(const char* flag, void* userdata, LuaHostArray_Ref params) = 0;		
	
	bool handle_lua2host_call(LuaHostArray_Ref data);
	virtual bool handle_lua2host_call(std::string& callType, LuaHostArray_Ref data);
	
protected:
	virtual void lcall_print(const char* msg) = 0;
	virtual void lcall_setTimer(int timerId, int delayTime, int fixTime) = 0;
	virtual bool lcall_invoke(const char* method, LuaHostArray_Ref data) = 0;
	virtual bool lcall_ainvoke(int callId, const char* method, LuaHostArray_Ref data) = 0;
	virtual void lcall_response(LuaHostArray_Ref data) = 0;
	
protected:
	lua_State* m_state;
};

#define LUAHOST_ID "_LH_HOST"

static class LuaHostPrototype* getHost(lua_State* L) {
	lua_pushstring( L , LUAHOST_ID);
	lua_rawget( L , LUA_REGISTRYINDEX );

	if( !lua_islightuserdata(L, -1))	
	{
		lua_pushstring( L , "invalid lua host object" );
		lua_error( L );
	}	
	LuaHostPrototype* host = (LuaHostPrototype*) lua_touserdata(L , -1);
	lua_pop(L,1);
	return host;
}

#define LUAHOST_FLAG "_LH_FLAG"

std::string getLuaFlag( lua_State * L , int idx)
{
	std::string r;
   if ( !lua_isuserdata( L , idx ) )
      return r;

   if ( lua_getmetatable( L , idx ) == 0 )
      return r;

   lua_pushstring( L , LUAHOST_FLAG );
   lua_rawget( L , -2 );

   if (lua_isnil( L, -1 ))
   {
      lua_pop( L , 2 );
      return r;
   }
   r = lua_tostring(L, -1);
   lua_pop( L , 2 );
   return r;
}

static int lua_hostload(lua_State *L) {	
	LuaHostPrototype* host = getHost(L);
	return host->lapi_load();
}

static int lua_hostcall(lua_State *L) {
	LuaHostPrototype* host = getHost(L);
	return host->lapi_call();
}

static int lua_hostclosure(lua_State *L) {
	LuaHostPrototype* host = getHost(L);
	return host->lapi_closure();
}

static struct luaL_Reg hostlib[] = {
  {"call", lua_hostcall},
  {"closure", lua_hostclosure},
  {NULL, NULL}
};


int luaopen_host_ext (lua_State *L) {
  luaL_openlib(L, "luahost", hostlib, 0);  
  return 1;
}

static int luaToString( lua_State* L )
{
	LuaHostPrototype* host = getHost(L);
	if(host==NULL)return 0;	
	std::string flag = getLuaFlag(L, 1);
	if(flag.size()==0)return 0;
	void* data = lua_touserdata( L , 1 );
	std::string r = host->lapi_tostring(flag.c_str(), data);
	if(r.size()==0) {
		lua_pushfstring(L,"unknow userdata: %p", lua_touserdata(L, 1));
		return 1;	
	}
	lua_pushstring(L,r.c_str());
	return 1;
}

static int luaMemGC( lua_State * L )
{
	LuaHostPrototype* host = getHost(L);
	if(host==NULL)return 0;	
	std::string flag = getLuaFlag(L, 1);
	if(flag.size()==0)return 0;
	void* data = lua_touserdata( L , 1 );
	host->lapi_ud_memgc(flag.c_str(), data);	
	return 0;
}

static int luaIndex( lua_State * L )
{
	LuaHostPrototype* host = getHost(L);
	if(host==NULL)return 0;	
	std::string flag = getLuaFlag(L, 1);
	if(flag.size()==0)return 0;
	void* data = lua_touserdata( L , 1 );
	const char* key = lua_tostring(L, 2);
	return host->lapi_ud_index(flag.c_str(), data, key);	
}

static int luaCall( lua_State * L )
{
	LuaHostPrototype* host = getHost(L);
	if(host==NULL)return 0;		
	return host->lapi_ud_call();	
}

static void pushLuaValue(lua_State* L, LuaHostValuePrototype* vp, LuaHostValue v)
{
	int t = vp->getLuaType(v);
	if(t==LUA_TINT) {
		lua_pushinteger(L, vp->intValue(v));	
		return;
	} else if(t==LUA_TSTRING) {
		lua_pushstring(L, vp->stringValue(v).c_str());
		return;
	} else if(t==LUA_TBOOLEAN) {
		lua_pushboolean(L, vp->booleanValue(v)?1:0);
		return;
	} else if(t==LUA_TNUMBER) {
		lua_pushnumber(L, vp->numberValue(v));
		return;
	} else if(t==LUA_TARRAY) {
		LuaHostArray o = vp->arrayValue(v);
		lua_newtable(L);
		int i = 1;		
		LuaHostArrayIterator it = vp->arrayBegin(o);
		while(vp->arrayValid(o, it)) {
			LuaHostValue v2 = vp->arrayGetAt(o, it);
			
			lua_pushnumber(L, i+1);
			pushLuaValue(L, vp, v2);
			lua_settable(L,-3);
			
			vp->arrayNext(o, it);
		}
		return;
	} else if(t==LUA_TMAP) {
		LuaHostMap o = vp->mapValue(v);
		lua_newtable(L);
		LuaHostMapIterator it = vp->mapBegin(o);
		while(vp->mapValid(o, it)) {
			std::string key = vp->mapGetKey(o, it);
			LuaHostValue v2 = vp->mapGetValue(o, it);
			
			lua_pushstring(L, key.c_str());
			pushLuaValue(L, vp, v2);
			lua_settable(L,-3);

			vp->mapNext(o, it);
		}
		return;
	} else if(t==LUA_TUSERDATA) {
		vp->pushUserdata(L, v);
		return;
	}
	lua_pushnil(L);
}

static int pushStackData(lua_State* L, LuaHostValuePrototype* vp,LuaHostArray_Ref o) {
	LuaHostArrayIterator it = vp->arrayBegin(o);
	while(vp->arrayValid(o, it)) {
		LuaHostValue v2 = vp->arrayGetAt(o, it);		
		pushLuaValue(L, vp, v2);		
		vp->arrayNext(o, it);
	}
	return o.size();
}

static LuaHostValue popLuaValue(lua_State* L, LuaHostValuePrototype* vp, int idx) {
	int type = lua_type(L,idx);		
	switch(type) {
		case LUA_TBOOLEAN: {
			return vp->newBoolean(lua_toboolean(L,idx)!=0);
		}
		case LUA_TNUMBER: {
			lua_Integer v1 = lua_tointeger(L,idx);
			lua_Number v2 = lua_tonumber(L,idx);
			if(v1==v2) {
				return vp->newInt((int) v1);
			} else {
				return vp->newNumber((double) v2);
			}				
		}
		case LUA_TSTRING: {
			const char* v = lua_tostring(L, idx);			
			return vp->newString(v);
		}		
		case LUA_TTABLE: {				
			if(idx<0) {
				idx = lua_gettop(L)+idx+1;
			}

			int len = isLuaArray(L, idx);
			if (len > 0) {
				LuaHostArray r = vp->newArray();
				for (int i = 1; i <= len; i++) {		
					lua_rawgeti(L, idx, i);
					vp->arrayAdd(r, popLuaValue(L, vp, -1));
					lua_pop(L, 1);
				}
				return vp->newArrayValue(r);
			} else {
				LuaHostMap r = vp->newMap();
				lua_pushnil(L);
				while (lua_next(L, idx) != 0) {
					const char* key = lua_tostring(L, -2);
					if(key!=NULL) {					
						vp->mapSet(r, key, popLuaValue(L, vp, -1));
					}
					lua_pop(L, 1);
				}
				return vp->newMapValue(r);
			}
		}
		case LUA_TFUNCTION: {			
			lua_getfield(L, LUA_GLOBALSINDEX, LUA_FUNCTION_HOST_CLOSURE_ADD);
			lua_pushvalue(L , idx );			
			lua_call(L,1,1);
			int callId = lua_tointeger(L,-1);
			lua_pop(L,1);
			if(callId==0) {		
				return vp->newNull();
			}
			LuaHostPrototype* host = getHost(L);
			return host->lapi_createClosure(callId);
		}
		case LUA_TUSERDATA: {
			std::string flag = getLuaFlag(L, idx);
			if(flag.size()>0) {
				void* data = lua_touserdata( L , idx );
				return vp->popUserdata(L, flag.c_str(), data);
			}
		}
		case LUA_TNIL:
		default:		
			return vp->newNull();
	}
}

static void popStackData(lua_State* L, LuaHostValuePrototype* vp, LuaHostArray& params,int nresults) {
	int top = lua_gettop(L) - nresults;
	for(int i=1;i<=nresults;i++) {
		vp->arrayAdd(params, popLuaValue(L,vp,top+i));
	}
	if(nresults>0) {
		lua_pop(L,nresults);
	}	
}

// LuaHostPrototype
LuaHostPrototype::LuaHostPrototype()
	: m_state(NULL)
{
	
}

LuaHostPrototype::~LuaHostPrototype() 
{
	close();
}

void LuaHostPrototype::open()
{
    if (isOpen()) {
		return;
	}
	m_state = ::luaL_newstate();
    openHostLibs();
}

void LuaHostPrototype::close()
{
	if (isOpen())
    {
		lua_State* L = m_state;
		if(L!=NULL) {
			::lua_close(L);			
		}
        m_state = NULL;		
	}
}

void LuaHostPrototype::openHostLibs()
{
	lua_State* L = m_state;
	::luaL_openlibs(L);  /* open libraries */
	::luamodule_cjson(L);
	::luamodule_cjson_safe(L);
	::luamodule_md5(L);
	::luamodule_des56(L);
	::luaopen_host_ext(L);

	lua_pushstring( L , LUAHOST_ID);
	lua_pushlightuserdata(L, this);
	lua_settable( L , LUA_REGISTRYINDEX );		
}

void LuaHostPrototype::addpath(const char* path)
{
	::addSearchPath( m_state , path);
}

void LuaHostPrototype::setvar(const char* key, const char* value)
{
	::lua_pushstring(m_state, value);
	::lua_setfield(m_state, LUA_GLOBALSINDEX, key);
}

bool LuaHostPrototype::pcall(const char* fname, LuaHostArray_Ref data)
{
	lua_State* L = m_state;
	LuaHostValuePrototype* vp = valuePrototype();
	
	int top = lua_gettop(L);

	/* function to be called */
	lua_getfield(L, LUA_GLOBALSINDEX, fname);
	int sz = pushStackData(L, vp, data);
	int err = lua_pcall(L, sz, LUA_MULTRET, 0);	
	
	vp->resetArray(data);
	int ntop = lua_gettop(L);
	popStackData(L, vp, data, ntop - top);
	if(err==0) {
		return true;
	}	
	return false;
}

std::string LuaHostPrototype::eval(const char* content)
{
	lua_State* L = m_state;
	int err = luaL_dostring(L, content);
	if(err!=0) {
		const char* s = lua_tostring(L,-1);
		lua_pop(L,1);
		return std::string(s);
	}
	return std::string("");
}

bool LuaHostPrototype::buildCallError(LuaHostArray_Ref r, const char * format, ...)
{
	char buf[1024];
	va_list arglist;
	va_start(arglist, format);
	_vsnprintf(buf, 1024, format, arglist);
	va_end(arglist);
	
	LuaHostValuePrototype* vp = valuePrototype();
	vp->resetArray(r);
	vp->arrayAdd(r, vp->newString(buf));
    return false;
}

bool LuaHostPrototype::handle_lua2host_call(LuaHostArray_Ref data)
{
	lua_State* L = m_state;
	LuaHostValuePrototype* vp = valuePrototype();
	
	if(vp->arraySize(data)==0) {
		return buildCallError(data, "invalid call type");
	}
	LuaHostValue vCallType = vp->arrayPopFirst(data);
	std::string callType = vp->stringValue(vCallType);
	return handle_lua2host_call(callType, data);
}
	
bool LuaHostPrototype::handle_lua2host_call(std::string& callType, LuaHostArray_Ref data)
{
	lua_State* L = m_state;
	LuaHostValuePrototype* vp = valuePrototype();
	
	if (callType.compare("invoke")==0) {
		if(vp->arraySize(data)<1) {
			return buildCallError(data, "invalid method");
		}
		LuaHostValue v_method = vp->arrayPopFirst(data);		
		std::string method = vp->stringValue(v_method);
		return lcall_invoke(method.c_str(), data);
	} else if (callType.compare("ainvoke")==0) {
		if(vp->arraySize(data)<2) {
			return buildCallError(data, "invalid callId & method");
		}		
		LuaHostValue v_cid = vp->arrayPopFirst(data);
		LuaHostValue v_method = vp->arrayPopFirst(data);
		int cid = vp->intValue(v_cid);
		std::string method = vp->stringValue(v_method);
		
		if(cid<=0) {
			return buildCallError(data, "invalid syn call '%s[%d]'", method.c_str(), cid);
		}
		
		return lcall_ainvoke(cid, method.c_str(), data);
	} else if (callType.compare("timer")==0) {
        if (vp->arraySize(data) < 3)
        {
            return buildCallError(data, "invalid param (timerId,fix,delay)");
        }
		LuaHostValue v_tid = vp->arrayGetAt(data,1);
		int tid = vp->intValue(v_tid);
		if(tid<=0) {
			return buildCallError(data, "invalid timerId '%d'", tid);
		}

		LuaHostValue t_fix = vp->arrayGetAt(data,2);
		LuaHostValue t_delay = vp->arrayGetAt(data,3);
			
		int fix = vp->intValue(t_fix);
		int delay = vp->intValue(t_delay);
		if (delay < 0) {
			return buildCallError(data, "invalid timer delay");
		}
		lcall_setTimer(tid,delay,fix);
		return true;
	
	} else if (callType.compare("response")==0) {
		lcall_response(data);
		vp->resetArray(data);
        return true;		
	} else if (callType.compare("print")==0) {
		if(vp->arraySize(data)>0) {
			LuaHostValue tmpv = vp->arrayGetAt(data,1);
			std::string msg = vp->stringValue(tmpv);
			lcall_print(msg.c_str());
		}
		vp->resetArray(data);
        return true;
	}
	std::string s = "invalid callType '";
	s += callType;
	s += "'";
	return buildCallError(data, s.c_str()); 
}

bool LuaHostPrototype::reponseLuaAInvoke(int callId, const char* err, LuaHostArray_Ref data)
{	
	LuaHostValuePrototype* vp = valuePrototype();
	if(err==NULL) {
		vp->arrayPushFirst(data, vp->newNull());
	} else {
		vp->arrayPushFirst(data, vp->newString(err));
	}
	vp->arrayPushFirst(data, vp->newInt(callId));
	return pcall(LUA_FUNCTION_HOST_RESPONSE, data);
}

void LuaHostPrototype::pushUserdata(const char* flag, void* data, int size,bool canIndex, bool canCall)
{
	lua_State* L = m_state;
	if(data==NULL) {
		lua_pushnil(L);
		return;
	}

	void* userData = lua_newuserdata( L , size );
	memcpy(userData, data, size);

	/* Creates metatable */
	lua_newtable( L );

	if(canIndex) {
		/* pushes the __index metamethod */
		lua_pushstring( L , "__index" );
		lua_pushcfunction( L , &luaIndex );
		lua_rawset( L , -3 );
	}
	
	/* pusher the __gc metamethod */
	lua_pushstring( L , "__gc");
	lua_pushcfunction( L , &luaMemGC );
	lua_rawset( L , -3 );

	/* pusher the __tostring metamethod */
	lua_pushstring( L , "__tostring");
	lua_pushcfunction( L , &luaToString );
	lua_rawset( L , -3 );	

	if(canCall) {
		/* pusher the __call metamethod */
		lua_pushstring( L , "__call");
		lua_pushcfunction( L , &luaCall);
		lua_rawset( L , -3 );
	}

	lua_pushstring( L , LUAHOST_FLAG);
	lua_pushstring( L , flag );
	lua_rawset( L , -3 );

	lua_setmetatable( L , -2 );
}

int LuaHostPrototype::lapi_call()
{
	lua_State* L = m_state;
	LuaHostValuePrototype* vp = valuePrototype();
	
	int pc = lua_gettop(L);
	LuaHostArray data = vp->newArray();
	popStackData(L, vp, data, pc);
	try {		
		if(handle_lua2host_call(data)) {
			int r = pushStackData(L, vp, data);
			vp->releaseArray(data);
			return r;
		} else {
			LuaHostValue v_err = vp->arrayGetAt(data, 1);
			vp->releaseArray(data);
			lua_pushstring(L, vp->stringValue(v_err).c_str());
			lua_error(L);
			return 1;
		}
	} catch(std::string* err) {
		vp->releaseArray(data);
		lua_pushstring(L, err->c_str());
		delete err;
		lua_error(L);
		return 1;
	}
}

int LuaHostPrototype::lapi_closure()
{
	lua_State* L = m_state;
	if(!lua_isfunction(L, 1)) {
		return 0;
	}
	// ValuePrototype also use these codes
	lua_getfield(L, LUA_GLOBALSINDEX, LUA_FUNCTION_HOST_CLOSURE_ADD);
	lua_insert(L, 1);
	lua_call(L,1,1);
	int callId = lua_tointeger(L,1);
	lua_pop(L,1);
	if(callId==0) {		
		return 0;
	}	
	LuaHostValue v = lapi_createClosure(callId);
	pushLuaValue(L, valuePrototype(), v);
	lua_pushinteger(L, callId);
	return 2;
}

int LuaHostPrototype::lapi_ud_call()
{
	lua_State* L = m_state;
	LuaHostValuePrototype* vp = valuePrototype();
	
	int pc = lua_gettop(L)-1;
	LuaHostArray params = vp->newArray();
	popStackData(L, vp, params, pc);

	std::string flag = getLuaFlag(L, 1);
	if(flag.size()==0) {
		vp->releaseArray(params);
		return 0;
	}
	void* data = lua_touserdata( L , 1 );
	try {		
		int r = lapi_userdataCall(flag.c_str(), data, params);
		vp->releaseArray(params);
		return r;
	} catch(std::string* err) {
		vp->releaseArray(params);
		lua_pushstring(L, err->c_str());
		delete err;
		lua_error(L);
		return 1;
	}
}

void LuaHostPrototype::pushValue(LuaHostValue v)
{
	pushLuaValue(m_state, valuePrototype(), v);
}

void LuaHostPrototype::enableModuleLoader()
{
	setModuleLoader(m_state, lua_hostload, 1);
}
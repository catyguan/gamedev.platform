#include "CCELuaHost.h"

USING_NS_CC;

// CCELuaValue
std::string CCELuaValue::EMPTY;

const CCELuaValue CCELuaValue::nullValue()
{
	CCELuaValue value;
    value.m_type = CCELuaValueTypeNull;
    value.m_field.intValue = 0;
    return value;
}

const CCELuaValue CCELuaValue::intValue(const int intValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeInt;
    value.m_field.intValue = intValue;
    return value;
}

const CCELuaValue CCELuaValue::numberValue(const double numberValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeNumber;
    value.m_field.numberValue = numberValue;
    return value;
}

const CCELuaValue CCELuaValue::booleanValue(const bool booleanValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeBoolean;
    value.m_field.booleanValue = booleanValue;
    return value;
}

const CCELuaValue CCELuaValue::stringValue(const char* stringValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeString;
    value.m_field.stringValue = new std::string(stringValue);
    return value;
}

const CCELuaValue CCELuaValue::stringValue(const std::string& stringValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeString;
    value.m_field.stringValue = new std::string(stringValue);
    return value;
}

const CCELuaValue CCELuaValue::tableValue(const CCELuaValueTable& dictValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeTable;
    value.m_field.tableValue = new CCELuaValueTable(dictValue);
    return value;
}

const CCELuaValue CCELuaValue::arrayValue(const CCELuaValueArray& arrayValue)
{
    CCELuaValue value;
    value.m_type = CCELuaValueTypeArray;
    value.m_field.arrayValue = new CCELuaValueArray(arrayValue);
    return value;
}

CCELuaValue::CCELuaValue(const CCELuaValue& rhs)
{
    copy(rhs);
}

CCELuaValue& CCELuaValue::operator=(const CCELuaValue& rhs)
{
    if (this != &rhs) copy(rhs);
    return *this;
}

CCELuaValue::~CCELuaValue(void)
{
    if (m_type == CCELuaValueTypeString)
    {
        delete m_field.stringValue;
    }
    else if (m_type == CCELuaValueTypeTable)
    {
        delete m_field.tableValue;
    }
    else if (m_type == CCELuaValueTypeArray)
    {
        delete m_field.arrayValue;
    }    
}

void CCELuaValue::copy(const CCELuaValue& rhs)
{
    memcpy(&m_field, &rhs.m_field, sizeof(m_field));
    m_type = rhs.m_type;
    if (m_type == CCELuaValueTypeString)
    {
        m_field.stringValue = new std::string(*rhs.m_field.stringValue);
    }
    else if (m_type == CCELuaValueTypeTable)
    {
        m_field.tableValue = new CCELuaValueTable(*rhs.m_field.tableValue);
    }
    else if (m_type == CCELuaValueTypeArray)
    {
        m_field.arrayValue = new CCELuaValueArray(*rhs.m_field.arrayValue);
    }    
}

// CCELuaHost
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

static CCELuaHost* getHost(lua_State* L) {
	lua_pushstring( L , "_cocos2dhost");
	lua_rawget( L , LUA_REGISTRYINDEX );

	if( !lua_islightuserdata(L, -1))	
	{
		lua_pushstring( L , "invalid _cocos2d host object" );
		lua_error( L );
	}	
	CCELuaHost* host = (CCELuaHost*) lua_touserdata(L , -1);
	lua_pop(L,1);
	return host;
}

static int lua_hostloader(lua_State *L) {	
	CCELuaHost* host = getHost(L);
	return host->luaLoad(L);
}

static int lua_hostcall(lua_State *L) {
	CCELuaHost* host = getHost(L);
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

static void pushLuaValue(lua_State* L, const CCELuaValue* v)
{
	if(v==NULL) {
		lua_pushnil(L);
		return;
	}
	CCELuaValueType t = v->getType();
	if(t==CCELuaValueTypeInt) {
		lua_pushinteger(L, v->intValue());	
		return;
	}		
	if(t==CCELuaValueTypeString) {
		lua_pushstring(L, v->stringValue().c_str());
		return;
	}
	if(t==CCELuaValueTypeBoolean) {
		lua_pushboolean(L, v->booleanValue()?1:0);
		return;
	}
	if(t==CCELuaValueTypeNumber) {
		lua_pushnumber(L, v->numberValue());
		return;
	}
	if(t==CCELuaValueTypeArray) {
		const CCELuaValueArray& o = v->arrayValue();
		lua_newtable(L);
		int i = 1;
		for (CCELuaValueArrayIterator it = o.begin(); it != o.end(); ++ it)
		{	
			lua_pushnumber(L, i+1);
			pushLuaValue(L, &(*it));
			lua_settable(L,-3);
		}
		return;
	}
	if(t==CCELuaValueTypeTable) {
		const CCELuaValueTable& o = v->tableValue();
		lua_newtable(L);
		for(CCELuaValueTableIterator it = o.begin(); it != o.end(); it++)
		{
			lua_pushstring(L, it->first.c_str());
			pushLuaValue(L, &(it->second));
			lua_settable(L,-3);
		}
		return;
	}
	lua_pushnil(L);	
}

static int pushStackData(lua_State* L, CCELuaValueArray& params) {
	int i = 0;	 
	for (CCELuaValueArrayIterator it = params.begin(); it != params.end(); ++ it)
	{	
		i++;
		pushLuaValue(L, &(*it));
	}
	return i;
}

static CCELuaValue popLuaValue(lua_State* L, int idx) {
	int type = lua_type(L,idx);		
	switch(type) {
		case LUA_TBOOLEAN: {
			return CCELuaValue::booleanValue(lua_toboolean(L,idx)!=0);
		}
		case LUA_TNUMBER: {
			lua_Integer v1 = lua_tointeger(L,idx);
			lua_Number v2 = lua_tonumber(L,idx);
			if(v1==v2) {
				return CCELuaValue::intValue((int) v1);
			} else {
				return CCELuaValue::numberValue((double) v2);
			}				
		}
		case LUA_TSTRING: {
			const char* v = lua_tostring(L, idx);			
			return CCELuaValue::stringValue(v);
		}		
		case LUA_TTABLE: {				
			if(idx<0) {
				idx = lua_gettop(L)+idx+1;
			}
			int len = isLuaArray(L, idx);
			if (len > 0) {
				CCELuaValueArray r;
				for (int i = 1; i <= len; i++) {		
					lua_rawgeti(L, idx, i);
					r.push_back(popLuaValue(L, -1));
					lua_pop(L, 1);
				}
				return CCELuaValue::arrayValue(r);
			} else {
				CCELuaValueTable r;
				lua_pushnil(L);
				while (lua_next(L, idx) != 0) {
					const char* key = lua_tostring(L, -2);
					if(key!=NULL) {					
						std::string s = key;
						r[s] = popLuaValue(L, -1);						
					}
					lua_pop(L, 1);
				}
				return CCELuaValue::tableValue(r);
			}
		}
		case LUA_TNIL:
		default:		
			return CCELuaValue::nullValue();
	}
}

static void popStackData(lua_State* L, CCELuaValueArray& params,int top,int nresults) {
	for(int i=1;i<=nresults;i++) {		
		params.push_back(popLuaValue(L,top+i));
	}
	if(nresults>0) {
		lua_pop(L,nresults);
	}	
}

static CCELuaHost* s_pSharedLuaHost = NULL;

CCELuaHost::CCELuaHost()
{
	state = NULL;
	loader = NULL;
	loaderData = NULL;

	minWaitTime = -1;
	startTick = -1;
	nowTick = 0;
}

CCELuaHost::~CCELuaHost() 
{
	if(isOpen()) {
		close();
	}
}

void CCELuaHost::open()
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

void CCELuaHost::close()
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

void CCELuaHost::addpath(const char* path)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(path!=NULL);
	::addSearchPath( state , path);
}

void CCELuaHost::setvar(const char* key, const char* value)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(key!=NULL);
	CC_ASSERT(value!=NULL);

	::lua_pushstring(state, value);
	::lua_setfield(state, LUA_GLOBALSINDEX, key);
}

bool CCELuaHost::pcall(const char* fun, CCELuaValueArray& data)
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

std::string CCELuaHost::eval(const char* content)
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
	return CCELuaValue::EMPTY;
}

void CCELuaHost::setLoader(CCELuaLoader loader, void* data)
{
	CC_ASSERT(isOpen());

	this->loader = loader;
	this->loaderData = data;

	setModuleLoader(state, lua_hostloader, 1);
}

int CCELuaHost::luaLoad(lua_State* L)
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

void CCELuaHost::setCall(const char* name, CCELuaCall call, void* data)
{
	CC_ASSERT(name);
	CCELuaCallItem c;
	c.call = call;
	c.data = data;
	calls[name] = c;
}

bool CCELuaHost::luaCallError(CCELuaValueArray& r, const char * format, ...)
{
	char buf[1024];
	va_list arglist;
	va_start(arglist, format);
	_vsnprintf(buf, 1024, format, arglist);
	va_end(arglist);
	
	r.clear();
	r.push_back(CCELuaValue::stringValue(buf));
    return false;
}

bool CCELuaHost::handleCallback(lua_State* L, CCELuaValueArray& data)
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
			data.insert(data.begin(), CCELuaValue::booleanValue(true));
		} else {
			data.clear();
			if(cid<=0) {
				return luaCallError(data, "invalid syn call '%s[%d]'", method.c_str(), cid);
			}
			data.push_back(CCELuaValue::booleanValue(false));
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

int CCELuaHost::luaCallback(lua_State* L)
{
	CCELuaValueArray data;
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

void CCELuaHost::setInstance(CCELuaHost* pInstance)
{
    CC_ASSERT(s_pSharedLuaHost==NULL);
	s_pSharedLuaHost = pInstance;
}

CCELuaHost* CCELuaHost::sharedLuaHost(void)
{
	CC_ASSERT(s_pSharedLuaHost!=NULL);    
    return s_pSharedLuaHost;
}

void CCELuaHost::purgeSharedLuaHost(void)
{
    if (s_pSharedLuaHost)
    {
        delete s_pSharedLuaHost;
        s_pSharedLuaHost = NULL;
    }
}

bool CCELuaHost::callResponse(int callId, const char* err, CCELuaValueArray& data)
{	
	if(err==NULL) {
		data.insert(data.begin(), CCELuaValue::nullValue());
	} else {
		data.insert(data.begin(), CCELuaValue::stringValue(err));
	}
	data.insert(data.begin(), CCELuaValue::intValue(callId));
	return pcall("luaCallResponse", data);
}

void CCELuaHost::enablePrintLog()
{
	CC_ASSERT(isOpen());
	std::string err = eval("hostOrgPrint = print\n print = function(...)\n local msg = \"\"\n for i=1, arg.n do msg = msg .. tostring(arg[i])..\"\\t\" end\n c2dx.call(\"print\", msg)\n end\n");
	err.c_str();
}

void CCELuaHost::appRunnable(void* data, long mstick)
{
	CC_ASSERT(data);
	CCELuaHost* host = (CCELuaHost*) data;
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
				CCELuaValueArray ps;
				ps.push_back(CCELuaValue::intValue(cit->id));
				ps.push_back(CCELuaValue::intValue(cit->fix));
				bool r = host->pcall("luaTimerResponse", ps);
				if(r) {
					CCLOG("[LuaHost] timer[%d] => done - %s", cit->id, (ps.size()>0?ps[0].booleanValue():false)?"true":"false");
				} else {
					CCLOG("[LuaHost] timer[%d] => fail - %s", cit->id, (ps.size()>0?ps[0].stringValue():CCELuaValue::EMPTY).c_str());
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
}

void CCELuaHost::require(const char* package)
{
	CCELuaValueArray ps;
	ps.push_back(CCELuaValue::stringValue(package));
	pcall("require", ps);
}

void CCELuaHost::createApp(const char* appType, 
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
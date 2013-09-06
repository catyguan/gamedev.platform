#include "CCELuaApplication.h"
#include "cocoa/CCValueSupport.h"
#include "CCEUtil.h"

USING_NS_CC;

typedef CCValue LuaHostValue;
typedef CCValue& LuaHostValue_Ref;
typedef CCValueArray LuaHostArray;
typedef CCValueArray& LuaHostArray_Ref;
typedef CCValueArrayIterator LuaHostArrayIterator;
typedef LuaHostArrayIterator& LuaHostArrayIterator_Ref;
typedef CCValueMap LuaHostMap;
typedef CCValueMap& LuaHostMap_Ref;
typedef CCValueMapIterator LuaHostMapIterator;
typedef CCValueMapIterator& LuaHostMapIterator_Ref;

#include "../lualibs/luahost/luahost_prototype.cpp"

#define FLAG_OBJECT "_FObject"
#define FLAG_OMCALL "_FOMCall"
#define FLAG_FUNCTION "_FFunction"
#define FLAG_OCALL "_FOCall"
#define FLAG_LUA_FUNCTION "_FLuaFunction"

class CCELuaHostValuePrototype : public LuaHostValuePrototype
{
protected:
	int getLuaType(CCValueType t) {
		switch(t) {
		case CCValueTypeInt:return LUA_TINT;
		case CCValueTypeString:return LUA_TSTRING;
		case CCValueTypeBoolean:return LUA_TBOOLEAN;
		case CCValueTypeNumber:return LUA_TNUMBER;
		case CCValueTypeArray:return LUA_TARRAY;
		case CCValueTypeMap:return LUA_TMAP;
		case CCValueTypeObject:
		case CCValueTypeFunction:
		case CCValueTypeObjectCall:
			return LUA_TUSERDATA;
		default:
			return LUA_TNIL;
		}
	};
public:
	LuaHostPrototype* host;
	CCValue VNULL;

public:
	virtual int getLuaType(LuaHostValue_Ref v) {
		CCValueType t = v.getType();
		return getLuaType(t);
	};
	
	virtual LuaHostArray newArray() {
		return CCValueArray();
	};
	virtual int arraySize(LuaHostArray_Ref array) {
		return array.size();
	};
	virtual LuaHostArrayIterator arrayBegin(LuaHostArray_Ref array) {
		return array.begin();
	};
	virtual bool arrayValid(LuaHostArray_Ref array, LuaHostArrayIterator& it) {
		return it!=array.end();
	};
	virtual bool arrayNext(LuaHostArray_Ref array, LuaHostArrayIterator& it) {
		if(it!=array.end()) {
			it++;
		}
		return it!=array.end();
	};
	virtual LuaHostValue_Ref arrayGetAt(LuaHostArray_Ref array, LuaHostArrayIterator_Ref it) {
		if(it!=array.end()) {
			return (LuaHostValue_Ref) *it;
		}
		return VNULL;
	};
	virtual LuaHostValue_Ref arrayGetAt(LuaHostArray_Ref array, int idx) {
		int i = idx-1;
		if(i>=0 && i<(int) array.size()) {
			return array.at(i);
		}
		return VNULL;
	};
	virtual void arrayAdd(LuaHostArray_Ref array, LuaHostValue_Ref v) {
		array.push_back(v);
	};
	virtual LuaHostValue arrayPopFirst(LuaHostArray_Ref array) {
		CCValueArrayIterator it = array.begin();
		if(it!=array.end()) {
			CCValue r = *it;
			array.erase(it);
			return r;
		}
		return CCValue::nullValue();
	};
	virtual void arrayPushFirst(LuaHostArray_Ref array,LuaHostValue_Ref v) {
		array.insert(array.begin(), v);
	};
	virtual void resetArray(LuaHostArray_Ref array) {
		array.clear();
	};
	virtual void releaseArray(LuaHostArray_Ref v) {
		v.clear();
	};
	
	virtual LuaHostMap newMap() {
		return CCValueMap();
	};
	virtual LuaHostMapIterator mapBegin(LuaHostMap_Ref map) {
		return map.begin();
	};
	virtual bool mapValid(LuaHostMap_Ref map, LuaHostMapIterator& it) {
		return it!=map.end();
	};
	virtual bool mapNext(LuaHostMap_Ref map, LuaHostMapIterator& it) {
		if(it!=map.end()) {
			it++;
		}
		return it!=map.end();
	};
	virtual std::string mapGetKey(LuaHostMap_Ref map, LuaHostMapIterator_Ref it) {
		return it->first;
	};
	virtual LuaHostValue_Ref mapGetValue(LuaHostMap_Ref map, LuaHostMapIterator_Ref it) {
		return (LuaHostValue_Ref) it->second;
	};
	virtual void mapSet(LuaHostMap_Ref map, const char* key, LuaHostValue_Ref v) {
		map[key] = v;
	};
	virtual void releaseMap(LuaHostMap_Ref map) {
		map.clear();
	};
	
	virtual int intValue(LuaHostValue_Ref v) {
		return v.intValue();
	};
	virtual std::string stringValue(LuaHostValue_Ref v) {
		return v.stringValue();
	};
	virtual bool booleanValue(LuaHostValue_Ref v) {
		return v.booleanValue();
	};
	virtual double numberValue(LuaHostValue_Ref v) {
		return v.numberValue();
	};
	virtual LuaHostArray_Ref arrayValue(LuaHostValue_Ref v) {
		return *v.arrayValue();
	};
	virtual LuaHostMap_Ref mapValue(LuaHostValue_Ref v) {
		return *v.mapValue();
	};
	
	virtual LuaHostValue newBoolean(bool v) {
		return CCValue::booleanValue(v);
	};
	virtual LuaHostValue newInt(int v) {
		return CCValue::intValue(v);
	};
	virtual LuaHostValue newNumber(double v) {
		return CCValue::numberValue(v);
	};
	virtual LuaHostValue newString(const char* v) {
		return CCValue::stringValue(v);
	};
	virtual LuaHostValue newArrayValue(LuaHostArray_Ref v) {
		return CCValue::arrayValue(v);
	};
	virtual LuaHostValue newMapValue(LuaHostMap_Ref v) {
		return CCValue::mapValue(v);
	};
	virtual LuaHostValue newNull() {
		return CCValue::nullValue();
	};
	
	virtual void pushUserdata(lua_State* L, LuaHostValue_Ref v) {
		CCValueType t = v.getType();
		if(t==CCValueTypeObject) {
			CCObject* obj = v.objectValue();
			obj->retain();			
			host->pushUserdata(FLAG_OBJECT, &obj, sizeof(CCObject*), true, true);	
			return;
		} else if(t==CCValueTypeFunction) {
			CC_FUNCTION_CALL fn = v.fcallValue();
			host->pushUserdata(FLAG_FUNCTION, &fn, sizeof(CC_FUNCTION_CALL), false, true);	
			return;
		} else if(t==CCValueTypeObjectCall) {
			const CCOCall* call = v.ocallValue();
			host->pushUserdata(FLAG_OCALL, (void*) call, sizeof(CCOCall), false, true);
			call->pObject->retain();
			return;
		}		
	};
	virtual LuaHostValue popUserdata(lua_State* L, const char* flag, const void* data) {
		if(strcmp(flag, FLAG_OBJECT)==0) {
			CCObject* obj = *((CCObject**) data);
			return CCValue::objectValue(obj);
		}
		if(strcmp(flag, FLAG_FUNCTION)==0) {
			CC_FUNCTION_CALL fn = *((CC_FUNCTION_CALL*) data);
			return CCValue::fcallValue(fn);
		}
		if(strcmp(flag, FLAG_OCALL)==0) {
			CCOCall* call = (CCOCall*) data;
			return CCValue::ocallValue(call->pObject, call->call);
		}
		return CCValue::nullValue();
	};
};

class CCELuaHost: public LuaHostPrototype
{
public:
	CCELuaHost(CCELuaApplication* app) {
		m_valuePrototype.host = this;
		m_app = app;
	};
	virtual ~CCELuaHost(){};

public:
	virtual int lapi_load();	
	virtual LuaHostValue lapi_createClosure(int callId);
	virtual std::string lapi_tostring(const char* flag, void* userdata);
	virtual void lapi_ud_memgc(const char* flag, void* userdata);
	virtual int lapi_ud_index(const char* flag, void* userdata, const char* key);
	
protected:
	virtual LuaHostValuePrototype* valuePrototype() {
		return &m_valuePrototype;
	};
	virtual int lapi_userdataCall(const char* flag, void* userdata, LuaHostArray_Ref params);		
	
	virtual bool handle_lua2host_call(std::string& callType, LuaHostArray_Ref data);
	
protected:
	virtual void lcall_print(const char* msg) {
		CCLOG("[LUA print] %s", msg);
	};
	virtual void lcall_setTimer(int timerId, int delayTime, int fixTime);
	virtual bool lcall_invoke(const char* method, LuaHostArray_Ref data);
	virtual bool lcall_ainvoke(int callId, const char* method, LuaHostArray_Ref data);
	virtual void lcall_response(LuaHostArray_Ref data);
	
protected:
	CCELuaApplication* m_app;
	CCELuaHostValuePrototype m_valuePrototype;
};

std::string CCELuaHost::lapi_tostring(const char* flag, void* userdata)
{
	if(strcmp(flag, FLAG_OBJECT ) == 0)
	{
		CCObject* obj = *((CCObject**) userdata);
		return StringUtil::format("CCVObject: %p", obj);
	}
	if (strcmp(flag, FLAG_OMCALL ) == 0) {
		char* p = (char*) userdata;
		return StringUtil::format("CCVOMethod: %s", p);
	}
	if ( strcmp( flag , FLAG_OCALL ) ==0 ) {
		CCOCall* call = (CCOCall*) userdata;
		return StringUtil::format("CCVOCall: %p,%p", call->pObject,call->call);
	}
	return "";
}

void CCELuaHost::lapi_ud_memgc(const char* flag, void* userdata)
{
	if ( strcmp(flag , FLAG_OBJECT ) == 0 )
	{
		CCObject* obj = *((CCObject**) userdata);
		if(obj!=NULL) {
			obj->release();
		}
		return;
	}
	if ( strcmp(flag, FLAG_OMCALL ) ==0 ) {		
		return;
	}
	if ( strcmp(flag , FLAG_OCALL ) ==0 ) {
		CCOCall* call = (CCOCall*) userdata;
		if(call->pObject!=NULL) {
			call->pObject->release();
		}
		return;
	}
	CCLOG("unknow userdata");
}

int CCELuaHost::lapi_userdataCall(const char* flag, void* userdata, LuaHostArray_Ref params)
{
	lua_State* L = m_state;
	CCValue r;
	if ( strcmp(flag, FLAG_OMCALL )==0 )
	{
		if(params.size()<1 || !params[0].isObject()) {
			lua_pushstring(L, "invalid object for method call");
			lua_error(L);
		}
		CCObject* obj = params[0].objectValue();
		params.erase(params.begin());
		char* p = (char*) userdata;			
		r = obj->call(p, params);		
	} else if ( strcmp(flag , FLAG_OBJECT)==0) {
		CCObject** ptr = (CCObject**) userdata;
		r = (*ptr)->invoke(params);		
	} else if ( strcmp(flag, FLAG_OCALL)==0 ) {
		CCOCall* ptr = (CCOCall*) userdata;
		r = (ptr->pObject->*ptr->call)(params);
	} else if( strcmp( flag , FLAG_FUNCTION)==0 ) {
		CC_FUNCTION_CALL* ptr = (CC_FUNCTION_CALL*) userdata;
		CC_FUNCTION_CALL fn = *(ptr);
		r = fn(params);
	} else {
		lua_pushstring( L , "invalid call" );
		lua_error( L );
	}
	pushValue(r);
	return 1;
}

int CCELuaHost::lapi_ud_index(const char* flag, void* userdata, const char* key)
{
	lua_State* L = m_state;
	if ( strcmp( flag , FLAG_OBJECT )!=0 )
	{
		lua_pushstring( L , "not CCObject" );
		lua_error( L );
	}
	CCObject* obj = *((CCObject**) userdata);	
	if(obj->canCall(key)) {
		int c = strlen(key)+1;
		char* buf = new char[c];
		strcpy(buf, key);
		pushUserdata(FLAG_OMCALL, buf, c, false, true);		
		delete buf;
		return 1;
	}
	CCObject* sobj = obj->findChildById(key);
	if(sobj!=NULL) {
		sobj->retain();			
		pushUserdata(FLAG_OBJECT, &sobj, sizeof(CCObject*), true, true);	
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int CCELuaHost::lapi_load()
{
	lua_State* L = m_state;
	if(m_app->m_loader==NULL) {
		lua_pushstring(L, "loader is null");
		return 1;
	}
	const char* lname = lua_tostring(L,-1);
	try{
		std::string content = m_app->m_loader(m_app, m_app->m_loaderData, lname);
		luaL_loadbuffer(L, content.c_str(), content.length(), lname);				
		return 1;				
	} catch(std::string* err){
		lua_pushstring(L, err->c_str());
		delete err;
		return 1;
	}
}

LuaHostValue CCELuaHost::lapi_createClosure(int callId)
{
	CCELuaClosureObject* obj = CCELuaClosureObject::create(m_app, callId);
	return CCValue::objectValue(obj);
}

bool CCELuaHost::handle_lua2host_call(std::string& callType, LuaHostArray_Ref data)
{
	if (callType.compare("application")==0) {
		data.clear();
		data.push_back(CCValue::objectValue(m_app));
		return true;
	}
	return LuaHostPrototype::handle_lua2host_call(callType, data); 
}

void CCELuaHost::lcall_setTimer(int timerId, int delayTime, int fixTime)
{
	CCLOG("setTimer - %d,%d,%d", timerId, delayTime, fixTime);
	CCELuaHostTimer ntimer;
	ntimer.id = timerId;
	ntimer.delay = delayTime;
	ntimer.fix = fixTime;
	ntimer.time = m_app->m_nowTick+delayTime;
	m_app->m_timers.push_back(ntimer);
	if(m_app->m_minWaitTime<0 || ntimer.time<m_app->m_minWaitTime) {
		m_app->m_minWaitTime = ntimer.time;
	}	
}

bool CCELuaHost::lcall_invoke(const char* method, LuaHostArray_Ref data)
{
	std::map<std::string, CCELuaCallItem>::const_iterator it = m_app->m_calls.find(method);
	if(it==m_app->m_calls.end()) {
		std::string emsg = StringUtil::format("invalid method '%s'", method);
		return buildCallError(data, emsg); 
	}
	return it->second.call(m_app, it->second.data, 0, data);
}

bool CCELuaHost::lcall_ainvoke(int callId, const char* method, LuaHostArray_Ref data)
{
	std::map<std::string, CCELuaCallItem>::const_iterator it = m_app->m_calls.find(method);
	if(it==m_app->m_calls.end()) {
		std::string emsg = StringUtil::format("invalid method '%s'", method);
		return buildCallError(data, emsg); 
	}
	return it->second.call(m_app, it->second.data, callId, data);
}

void CCELuaHost::lcall_response(LuaHostArray_Ref data)
{
	CC_ASSERT(false);
}

// CCELuaApplication
CCELuaApplication::CCELuaApplication()
{
	m_host = new CCELuaHost(this);
	
	m_loader = NULL;
	m_loaderData = NULL;

	m_minWaitTime = -1;
	m_startTick = -1;
	m_nowTick = 0;
	m_gcTick = 0;
}

CCELuaApplication::~CCELuaApplication() 
{
	if(m_host) {
		delete m_host;
		m_host = NULL;
	}
}

bool CCELuaApplication::isOpen() {
	return m_host->isOpen();
}

void CCELuaApplication::open()
{
	m_host->open();
}

void CCELuaApplication::cleanup()
{
	CCEApplication::cleanup();
	close();
}

void CCELuaApplication::close()
{
	m_host->close();
	m_calls.clear();
}

void CCELuaApplication::addpath(const char* path)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(path!=NULL);
	m_host->addpath(path);
}

void CCELuaApplication::setvar(const char* key, const char* value)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(key!=NULL);
	CC_ASSERT(value!=NULL);
	m_host->setvar(key,value);
}

bool CCELuaApplication::pcall(const char* fun, CCValueArray& data, CCValueArray& result)
{
	if(!isOpen()) {
		result.clear();
		result.push_back(CCValue::stringValue("luaState closed"));
		return false;
	}
	CC_ASSERT(fun!=NULL);
	bool r = m_host->pcall(fun,data, result);
	if(!r && result.size()>0) {
		std::string err = result.begin()->stringValue();
		CCLOG("pcall '%s' fail -> '%s'", fun, err.c_str());
	}
	return r;
}

std::string CCELuaApplication::eval(const char* content)
{
	CC_ASSERT(isOpen());
	CC_ASSERT(content!=NULL);
	return m_host->eval(content);
}

void CCELuaApplication::setLoader(CCELuaLoader loader, void* data)
{
	CC_ASSERT(isOpen());

	this->m_loader = loader;
	this->m_loaderData = data;

	m_host->enableModuleLoader();
}

void CCELuaApplication::setCall(const char* name, CCELuaCall call, void* data)
{
	CC_ASSERT(name);
	CCELuaCallItem c;
	c.call = call;
	c.data = data;
	m_calls[name] = c;
}

CCELuaApplication* CCELuaApplication::sharedLuaHost(void)
{
	return (CCELuaApplication*) sharedApp();
}

bool CCELuaApplication::callResponse(int callId, const char* err, CCValueArray& data)
{	
	return m_host->reponseLuaAInvoke(callId, err, data, data);
}

void CCELuaApplication::enablePrintLog()
{
	CC_ASSERT(isOpen());
	std::string err = eval("hostOrgPrint = print\n print = function(...)\n local msg = \"\"\n for i=1, arg.n do msg = msg .. tostring(arg[i])..\"\\t\" end\n luahost.call(\"print\", msg)\n end\n");
	err.c_str();
}

void CCELuaApplication::appRunnable(void* data, long mstick)
{
	CC_ASSERT(data);
	CCELuaApplication* host = (CCELuaApplication*) data;
	CC_ASSERT(host->isOpen());

	if(host->m_startTick==-1) {
		host->m_startTick = mstick;
	}
	host->m_nowTick = mstick - host->m_startTick;

	// CCLOG("[LuaHost] apprun %d", host->nowTick);
	if(host->m_minWaitTime>=0 && host->m_minWaitTime<host->m_nowTick) {
		int mintm = host->m_nowTick+3600*1000;
		for (std::list<CCELuaHostTimer>::const_iterator it = host->m_timers.begin(); it != host->m_timers.end(); )
		{	
			std::list<CCELuaHostTimer>::const_iterator cit = it;			

			if(cit->time <= host->m_nowTick) {
				CCValueArray ps;
				ps.push_back(CCValue::intValue(cit->id));
				ps.push_back(CCValue::intValue(cit->fix));
				bool r = host->pcall(LUA_FUNCTION_HOST_TIMER_RESPONSE, ps, ps);
				if(r) {
					CCLOG("[LuaHost] timer[%d] => done - %s", cit->id, (ps.size()>0?ps[0].booleanValue():false)?"true":"false");
				} else {
					CCLOG("[LuaHost] timer[%d] => fail - %s", cit->id, (ps.size()>0?ps[0].stringValue():std::string()).c_str());
				}
                if (r && cit->fix > 0)
                {
					if(ps.size()>0 && ps[0].booleanValue()) {
						CCELuaHostTimer ntimer;
						ntimer.id = cit->id;
						ntimer.delay = cit->delay;
						ntimer.fix = cit->fix;
						ntimer.time = cit->time+cit->fix;
						host->m_timers.push_back(ntimer);
					} else {
						CCLOG("[LuaHost] timer[%d] => stop timer", cit->id);
					}
                }
				++ it;
				host->m_timers.erase(cit);
			} else {
				if(cit->time < mintm) {
					mintm = cit->time;
				}
				++ it;
			}			
		}
		host->m_minWaitTime = mintm;
	}	
	host->m_host->gc(1);
}

void CCELuaApplication::require(const char* package)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(package));
	pcall("require", ps, ps);
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

// CCELuaCallResponse
CCELuaResponseObject::CCELuaResponseObject()
{
	m_App = NULL;
	m_callId = 0;
}

CCELuaResponseObject::~CCELuaResponseObject()
{

}

CCELuaResponseObject* CCELuaResponseObject::create(CCELuaApplication* app, int cid)
{
	CCValueArray ret;
	return create(app,cid,ret);
}

CCELuaResponseObject* CCELuaResponseObject::create(CCELuaApplication* app, int cid, CCValueArray& ret)
{
	CCELuaResponseObject* r = new CCELuaResponseObject();
	r->m_App = app==NULL?CCELuaApplication::sharedLuaHost():app;
	r->m_callId = cid;
	r->autorelease();
	r->m_Return.push_back(CCValue::intValue(cid));
	CCValueUtil::append(r->m_Return, ret);
	return r;
}

CCValue CCELuaResponseObject::invoke(CCValueArray& params)
{	
	CCValueArray result;
	bool r = false;
	if(m_Return.size()>0) {
		CCValueUtil::append(result,m_Return);
		CCValueUtil::append(result,params);	
		r = m_App->pcall("luaInvokeResponse", result, result);
	} else {
		m_App->pcall("luaInvokeResponse",params,result);
	}
	if(r) {
		return result.size()>0?result[0]:CCValue::nullValue();
	}
	throw new std::string(result.size()>0?result[0].stringValue():"<empty message>");
}

// CCELuaCallObject
CCELuaCallObject::CCELuaCallObject()
{
	m_App = NULL;
}

CCELuaCallObject::~CCELuaCallObject()
{

}

CCELuaCallObject* CCELuaCallObject::create(CCELuaApplication* app, const char* fun)
{
	CCValueArray ps;
	return create(app,fun,ps);
}

CCELuaCallObject* CCELuaCallObject::create(CCELuaApplication* app, const char* fun, CCValueArray& ps)
{
	CCELuaCallObject* r = new CCELuaCallObject();
	r->m_App = app==NULL?CCELuaApplication::sharedLuaHost():app;
	r->m_csFun = fun;
	r->autorelease();
	r->m_Params = ps;
	return r;
}

CCValue CCELuaCallObject::invoke(CCValueArray& params)
{
	CCValueArray result;
	bool r = false;
	if(m_Params.size()>0) {
		CCValueUtil::append(result,m_Params);
		CCValueUtil::append(result,params);	
		r = m_App->pcall(m_csFun.c_str(), result, result);
	} else {
		r = m_App->pcall(m_csFun.c_str(), params, result);
	}
	if(r) {
		return result.size()>0?result[0]:CCValue::nullValue();
	}
	throw new std::string(result.size()>0?result[0].stringValue():"<empty message>");
}


// CCELuaClosureObject
CCELuaClosureObject::CCELuaClosureObject()
	: m_App(NULL)
	, m_callId(0)
{
	m_App = NULL;
}

CCELuaClosureObject::~CCELuaClosureObject()
{
	if(m_App!=NULL && m_callId>0) {
		CCValueArray ps;
		ps.push_back(CCValue::intValue(m_callId));
		m_App->pcall(LUA_FUNCTION_HOST_CLOSURE_REMOVE, ps, ps);
	}
}

CCELuaClosureObject* CCELuaClosureObject::create(CCELuaApplication* app, int callId)
{
	CCELuaClosureObject* r = new CCELuaClosureObject();
	r->m_App = app==NULL?CCELuaApplication::sharedLuaHost():app;
	r->m_callId = callId;
	r->autorelease();
	return r;
}

CCValue CCELuaClosureObject::invoke(CCValueArray& params)
{
	CCValueArray ps;
	ps.push_back(CCValue::intValue(m_callId));
	CCValueUtil::append(ps,params);	
	bool r = m_App->pcall(LUA_FUNCTION_HOST_CLOSURE_CALL, ps, ps);
	if(r) {
		return ps.size()>0?ps[0]:CCValue::nullValue();
	}
	throw new std::string(ps.size()>0?ps[0].stringValue():"<empty message>");
}

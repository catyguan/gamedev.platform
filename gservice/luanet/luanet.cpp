// 这是主 DLL 文件。

#include "stdafx.h"

#include "luanet.h"
#include "../lualibs/luahost/luahost_prototype.cpp"

#define FLAG_OBJECT "_FObject"
#define FLAG_OCALL "_FOCall"

using namespace luanet;
using namespace Runtime::InteropServices;

std::string marshalString(System::String^ s) {      
	array<Byte>^ ar = System::Text::Encoding::UTF8->GetBytes(s);
	IntPtr p = Marshal::AllocHGlobal(ar->Length);
	Marshal::Copy(ar, 0, p, ar->Length);
	std::string r((const char*) p.ToPointer(),ar->Length);
	Marshal::FreeHGlobal(p);
	return r;
}

String^ marshalString(const char* s)
{
	int len = strlen(s);
	array< Byte >^ byteArray = gcnew array< Byte >(len);
	Marshal::Copy((IntPtr)(char*)s,byteArray, 0, len);
	return System::Text::Encoding::UTF8->GetString(byteArray);
}

class LuaHostValueP : public LuaHostValuePrototype
{
protected:
	int getLuaType(LuaValueType t) {
		switch(t) {
		case LuaValueType::Int:return LUA_TINT;
		case LuaValueType::String:return LUA_TSTRING;
		case LuaValueType::Boolean:return LUA_TBOOLEAN;
		case LuaValueType::Number:return LUA_TNUMBER;
		case LuaValueType::Array:return LUA_TARRAY;
		case LuaValueType::Map:return LUA_TMAP;
		case LuaValueType::Object:
		case LuaValueType::ObjectCall:
			return LUA_TUSERDATA;
		default:
			return LUA_TNIL;
		}
	};

public:
	LuaHostPrototype* host;
	
public:
	virtual int getLuaType(LuaHostValue_Ref v) {
		if(v==nullptr)return LUA_TNIL;
		LuaValueType t = v->getType();
		return getLuaType(t);
	};
	
	virtual LuaHostArray newArray() {
		return gcnew System::Collections::Generic::List<LuaValue^>();
	};
	virtual int arraySize(LuaHostArray_Ref array) {
		return array->Count;
	};
	virtual LuaHostArrayIterator arrayBegin(LuaHostArray_Ref array) {
		return array->GetEnumerator();
	};
	virtual bool arrayValid(LuaHostArray_Ref array, LuaHostArrayIterator_Ref it) {
		return it.isValid();
	};
	virtual bool arrayNext(LuaHostArray_Ref array, LuaHostArrayIterator_Ref it) {
		return it.next();
	};
	virtual LuaHostValue_Ref arrayGetAt(LuaHostArray_Ref array, LuaHostArrayIterator_Ref it) {
		LuaValue^ r = (LuaValue^) it.value();
		if(r==nullptr) {
			return LuaValue::nullValue();
		}
		return r;
	};
	virtual LuaHostValue_Ref arrayGetAt(LuaHostArray_Ref a, int idx) {
		int i = idx-1;
		if(i<a->Count) {
			return (LuaValue^) a[i];
		}
		return LuaValue::nullValue();
	};
	virtual void arrayAdd(LuaHostArray_Ref array, LuaHostValue_Ref v) {
		array->Add(v);
	};
	virtual LuaHostValue arrayPopFirst(LuaHostArray_Ref array) {
		if(array->Count>0) {
			LuaValue^ r = (LuaValue^) array[0];
			array->RemoveAt(0);
			return r;
		}
		return LuaValue::nullValue();
	};
	virtual void arrayPushFirst(LuaHostArray_Ref array,LuaHostValue_Ref v) {
		array->Insert(0, v);
	};
	virtual void resetArray(LuaHostArray_Ref array) {
		array->Clear();
	};
	virtual void releaseArray(LuaHostArray_Ref v) {
		v->Clear();
	};
	
	virtual LuaHostMap newMap() {
		return gcnew System::Collections::Generic::Dictionary<String^,LuaValue^>();
	};
	virtual LuaHostMapIterator mapBegin(LuaHostMap_Ref map) {
		return Iterator(map->GetEnumerator());
	};
	virtual bool mapValid(LuaHostMap_Ref map, LuaHostMapIterator_Ref it) {
		return it.isValid();
	};
	virtual bool mapNext(LuaHostMap_Ref map, LuaHostMapIterator_Ref it) {
		return it.next();
	};
	virtual std::string mapGetKey(LuaHostMap_Ref map, LuaHostMapIterator_Ref it) {
		String^ key = ((System::Collections::Generic::KeyValuePair<String^,LuaValue^>^)it.value())->Key;
		return marshalString(key);
	};
	virtual LuaHostValue_Ref mapGetValue(LuaHostMap_Ref map, LuaHostMapIterator_Ref it) {
		return ((System::Collections::Generic::KeyValuePair<String^,LuaValue^>^)it.value())->Value;
	};
	virtual void mapSet(LuaHostMap_Ref map, const char* key, LuaHostValue_Ref v) {
		map[marshalString(key)] = v;
	};
	virtual void releaseMap(LuaHostMap_Ref map) {
		map->Clear();
	};
	
	virtual int intValue(LuaHostValue_Ref v) {
		return v->intValue();
	};
	virtual std::string stringValue(LuaHostValue_Ref v) {
		return marshalString(v->stringValue());
	};
	virtual bool booleanValue(LuaHostValue_Ref v) {
		return v->booleanValue();
	};
	virtual double numberValue(LuaHostValue_Ref v) {
		return v->numberValue();
	};
	virtual LuaHostArray_Ref arrayValue(LuaHostValue_Ref v) {
		return v->arrayValue();
	};
	virtual LuaHostMap_Ref mapValue(LuaHostValue_Ref v) {
		return v->mapValue();
	};
	
	virtual LuaHostValue newBoolean(bool v) {
		return LuaValue::booleanValue(v);
	};
	virtual LuaHostValue newInt(int v) {
		return LuaValue::intValue(v);
	};
	virtual LuaHostValue newNumber(double v) {
		return LuaValue::numberValue(v);
	};
	virtual LuaHostValue newString(const char* v) {
		return LuaValue::stringValue(marshalString(v));
	};
	virtual LuaHostValue newArrayValue(LuaHostArray_Ref v) {
		return LuaValue::arrayValue(v);
	};
	virtual LuaHostValue newMapValue(LuaHostMap_Ref v) {
		return LuaValue::mapValue(v);
	};
	virtual LuaHostValue newNull() {
		return LuaValue::nullValue();
	};
	
	virtual void pushUserdata(lua_State* L, LuaHostValue_Ref v) {
		LuaValueType t = v->getType();
		if(t==LuaValueType::Object) {
			Object^ obj = v->objectValue();
			if(obj!=nullptr) {
				gcroot<LuaValue^>* pval = new gcroot<LuaValue^>(LuaValue::objectValue(obj));
				// System::Console::WriteLine("memnew {0}", obj);
				LuaObject^ lo = dynamic_cast<LuaObject^>(obj);				
				host->pushUserdata(FLAG_OBJECT, &pval, sizeof(pval), lo!=nullptr, lo!=nullptr);	
				return;
			}
		} else if(t==LuaValueType::ObjectCall) {
			Object^ obj = v->ocObject();
			if(obj!=nullptr) {
				gcroot<LuaValue^>* ptr = new gcroot<LuaValue^>(LuaValue::ocallValue(obj, v->ocName()));				
				host->pushUserdata(FLAG_OBJECT, &ptr, sizeof(ptr), false, true);	
				return;
			}
		}
		host->pushValue(LuaValue::nullValue());
	};
	virtual LuaHostValue popUserdata(lua_State* L, const char* flag, const void* data) {
		if(strcmp(flag, FLAG_OBJECT)==0) {
			gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) data);
			return LuaValue::objectValue((*pval)->objectValue());
		}
		if(strcmp(flag, FLAG_OCALL)==0) {
			gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) data);
			return LuaValue::ocallValue((*pval)->ocObject(),(*pval)->ocName());
		}
		return LuaValue::nullValue();
	};
};

class LuaHost: public LuaHostPrototype
{
public:
	LuaHost(LuaAppRealm^ app) {
		m_valuePrototype.host = this;
		m_app = app;
	};
	virtual ~LuaHost(){};

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
		String^ m("[LUA print] ");
		m += marshalString(msg);		
		System::Console::WriteLine(m);
	};

	virtual void lcall_setTimer(int timerId, int delayTime, int fixTime);
	virtual bool lcall_invoke(const char* method, LuaHostArray_Ref data);
	virtual bool lcall_ainvoke(int callId, const char* method, LuaHostArray_Ref data);
	virtual void lcall_response(LuaHostArray_Ref data);
	
protected:
	gcroot<LuaAppRealm^> m_app;
	LuaHostValueP m_valuePrototype;
};

std::string LuaHost::lapi_tostring(const char* flag, void* userdata)
{	
	if(strcmp(flag, FLAG_OBJECT ) == 0)
	{
		gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) userdata);
		Object^ obj = (*pval)->objectValue();
		if(obj!=nullptr) {
			return marshalString(obj->ToString());
		}
	}
	if ( strcmp( flag , FLAG_OCALL ) ==0 ) {
		gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) userdata);
		Object^ obj = (*pval)->ocObject();
		if(obj!=nullptr) {
			String^ r = String::Format("{0}::{1}", obj->ToString(), (*pval)->ocName());
			return marshalString(r);
		}
	}
	return "<unknow>";
}

void LuaHost::lapi_ud_memgc(const char* flag, void* userdata)
{
	if ( strcmp(flag , FLAG_OBJECT ) == 0 || strcmp(flag , FLAG_OCALL ) ==0)
	{
		gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) userdata);
		// if((*pval)->isObject()) {
		//	System::Console::WriteLine("memgc {0}", (*pval)->objectValue());
		// }
		delete pval;
		return;
	}
}

int LuaHost::lapi_userdataCall(const char* flag, void* userdata, LuaHostArray_Ref params)
{
	lua_State* L = m_state;
	LuaValue^ r;
	if ( strcmp(flag , FLAG_OBJECT)==0) {
		gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) userdata);
		Object^ obj = (*pval)->objectValue();
		if(obj!=nullptr) {
			LuaObject^ lo = dynamic_cast<LuaObject^>(obj);
			if(lo!=nullptr) {
				if(lo->canCall()) {
					try {
						if(lo->call(m_app, params)) {
							if(params->Count>0) {
								r = params[0];
							}
						} else {
							String^ err = params->Count>0?params[0]->stringValue():"<error LuaObject call>";
							lua_pushstring(L, marshalString(err).c_str());
							lua_error(L);
						}
					} catch(Exception^ err) {
						lua_pushstring(L, marshalString(err->Message).c_str());
						lua_error(L);
					}
				}
			}
		}		
	} else if ( strcmp(flag, FLAG_OCALL)==0 ) {
		gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) userdata);
		Object^ obj = (*pval)->ocObject();
		if(obj!=nullptr) {
			LuaObject^ lo = dynamic_cast<LuaObject^>(obj);
			if(lo!=nullptr) {
				String^ name = (*pval)->ocName();
				if(lo->canInvoke(name)) {
					try {
						if(lo->invoke(m_app, name, params)) {
							if(params->Count>0) {
								r = params[0];
							}
						} else {
							String^ err = params->Count>0?params[0]->stringValue():"<error at '"+name+"'>";
							lua_pushstring(L, marshalString(err).c_str());
							lua_error(L);
						}
					} catch(Exception^ err) {
						lua_pushstring(L, marshalString(err->Message).c_str());
						lua_error(L);
					}
				}
			}
		}
	} else {
		lua_pushstring( L , "invalid call" );
		lua_error( L );
	}
	pushValue(r);
	return 1;
}

int LuaHost::lapi_ud_index(const char* flag, void* userdata, const char* key)
{
	lua_State* L = m_state;
	if ( strcmp( flag , FLAG_OBJECT )!=0 )
	{
		lua_pushstring( L , "not LuaObject" );
		lua_error( L );
	}
	gcroot<LuaValue^>* pval = *((gcroot<LuaValue^>**) userdata);
	Object^ obj = (*pval)->objectValue();
	LuaObject^ lo = dynamic_cast<LuaObject^>(obj);
	if(lo!=nullptr) {
		String^ name = marshalString(key);
		if(lo->canInvoke(name)) {
			LuaValue^ oc = LuaValue::ocallValue(obj, name);			
			gcroot<LuaValue^>* p = new gcroot<LuaValue^>(oc);
			pushUserdata(FLAG_OCALL, &p, sizeof(p), false, true);			
			return 1;
		}
		if(lo->canGetValue(name)) {
			LuaValue^ v = lo->getValue(m_app, name);
			pushLuaValue(L,valuePrototype(),v);			
			return 1;
		}
	}
	lua_pushnil(L);	
	return 1;
}

int LuaHost::lapi_load()
{
	lua_State* L = m_state;
	const char* lname = lua_tostring(L,-1);

	try{
		std::string content = marshalString(m_app->lapi_load(marshalString(lname)));
		luaL_loadbuffer(L, content.c_str(), content.length(), lname);				
		return 1;				
	} catch(Exception^ err){
		std::string emsg = marshalString(err->Message);
		lua_pushstring(L, emsg.c_str());		
		return 1;
	}
}

LuaHostValue LuaHost::lapi_createClosure(int callId)
{
	LuaObjectClosure^ obj = gcnew LuaObjectClosure(callId);
	return LuaValue::objectValue(obj);
}

bool LuaHost::handle_lua2host_call(std::string& callType, LuaHostArray_Ref data)
{
	if (callType.compare("application")==0) {
		data->Clear();
		data->Add(LuaValue::objectValue(m_app));
		return true;
	}
	return LuaHostPrototype::handle_lua2host_call(callType, data); 
}

void LuaHost::lcall_setTimer(int timerId, int delayTime, int fixTime)
{
#ifdef _DEBUG
	System::Console::WriteLine("setTimer - {0},{1},{2}", timerId, delayTime, fixTime);
#endif
	if(m_app->timer==nullptr) {
		throw gcnew LuaError("app timer invalid");
	}
	m_app->timer->crreateTimer(m_app,timerId, delayTime<=0?fixTime:delayTime,fixTime);	
}

bool LuaHost::lcall_invoke(const char* method, LuaHostArray_Ref data)
{
	return m_app->lapi_invoke(marshalString(method),0, data);	
}

bool LuaHost::lcall_ainvoke(int callId, const char* method, LuaHostArray_Ref data)
{
	return m_app->lapi_invoke(marshalString(method),callId, data);	
}

void LuaHost::lcall_response(LuaHostArray_Ref data)
{
	// TODO
	// ASSERT(false);
}

namespace luanet {

	// LuaValue
	LuaValue::LuaValue()
	{
	}

	LuaValue::~LuaValue()
	{
	}

	LuaValue^ LuaValue::nullValue()
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Null;
		return r;
	}

	LuaValue^ LuaValue::intValue(int v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Int;
		r->m_valueInt = v;
		return r;
	}

	LuaValue^ LuaValue::numberValue(double v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Number;
		r->m_valueNumber = v;
		return r;
	}

	LuaValue^ LuaValue::booleanValue(bool v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Boolean;
		r->m_valueInt = v?1:0;
		return r;
	}

	LuaValue^ LuaValue::stringValue(char* v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::String;
		r->m_valueString = marshalString(v);
		return r;
	}

	LuaValue^ LuaValue::stringValue(String^ v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::String;
		r->m_valueString = v;
		return r;
	}
	
	LuaValue^ LuaValue::mapValue( LuaHostMap_Ref v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Map;
		r->m_valueMap = v;
		return r;
	}

	LuaValue^ LuaValue::arrayValue(LuaHostArray_Ref v)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Array;
		r->m_valueArray = v;
		return r;
	}

	LuaValue^ LuaValue::objectValue(Object^ v)
	{
		if(v==nullptr)return nullValue();
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::Object;
		r->m_valueObject = v;
		return r;
	}
	
	LuaValue^ LuaValue::ocallValue(Object^ v, String^ name)
	{
		LuaValue^ r = gcnew LuaValue();
		r->m_type = LuaValueType::ObjectCall;
		r->m_valueObject = v;
		r->m_valueString = name;
		return r;
	}

	// LuaAPp & LuaAppRealm
	LuaApp::LuaApp()
	{
		m_host = NULL;
		m_calls = gcnew System::Collections::Generic::Dictionary<String^, LuaCall^>();
	}

	LuaApp::~LuaApp() 
	{
		if(m_host) {
			delete m_host;
			m_host = NULL;
		}
	}

	bool LuaApp::isOpen() {
		if(m_host!=NULL)return m_host->isOpen();
		return false;
	}

	void LuaApp::open()
	{
		if(m_host==NULL) {
			m_host = new LuaHost((LuaAppRealm^)this);
		}
		m_host->open();
	}

	void LuaApp::close()
	{
		if(m_host) {
			LuaHost* p = m_host;
			m_host = NULL;
			p->close();
			delete p;
		}
		m_calls->Clear();
	}

	void LuaApp::addpath(String^ path)
	{
		m_host->addpath(marshalString(path).c_str());
	}

	void LuaApp::setvar(String^ key, String^ value)
	{		
		std::string k = marshalString(key);
		std::string v = marshalString(value);
		m_host->setvar(k.c_str(), v.c_str());
	}

	bool LuaApp::pcall(String^ fun, LuaHostArray_Ref data, LuaHostArray_Ref result)
	{	
		bool r = m_host->pcall(marshalString(fun).c_str(),data, result);
		if(!r && data->Count>0) {
			String^ err = ((LuaValue^)result[0])->stringValue();
#if _DEBUG
			String^ msg = String::Format("pcall '{0}' fail -> '{1}'", fun, err);
			System::Console::WriteLine(msg);
#endif
		}
		return r;
	}

	String^ LuaApp::eval(String^ content)
	{		
		return marshalString(m_host->eval(marshalString(content).c_str()).c_str());
	}

	void LuaApp::setLoader(LuaLoader^ loader)
	{
		this->m_loader = loader;
		m_host->enableModuleLoader();
	}

	void LuaApp::setCall(String^ name, LuaCall^ call)
	{		
		m_calls[name] = call;
	}

	bool LuaApp::callResponse(int callId, String^ err, LuaHostArray_Ref data)
	{		
		return m_host->reponseLuaAInvoke(callId, err==nullptr?NULL:marshalString(err).c_str(), data, data);
	}

	bool LuaApp::closureCall(int callId, LuaHostArray_Ref params)
	{
		params->Insert(0, LuaValue::intValue(callId));
		return pcall(LUA_FUNCTION_HOST_CLOSURE_CALL, params, params);
	}

	void LuaApp::closureRemove(int callId)
	{
		if(isOpen()) {
			LuaHostArray ps = gcnew System::Collections::Generic::List<LuaValue^>();
			ps->Add(LuaValue::intValue(callId));
			pcall(LUA_FUNCTION_HOST_CLOSURE_REMOVE, ps, ps);
		}
	}

	void LuaApp::enablePrintLog()
	{
		m_host->eval("hostOrgPrint = print\n print = function(...)\n local msg = \"\"\n for i=1, arg.n do msg = msg .. tostring(arg[i])..\"\\t\" end\n luahost.call(\"print\", msg)\n end\n");
	}

	void LuaApp::require(String^ package)
	{
		LuaHostArray ps = gcnew System::Collections::Generic::List<LuaValue^>();
		ps->Add(LuaValue::stringValue(package));
		pcall("require", ps, ps);
	}

	void LuaApp::createApp(String^ appType, 
			System::Collections::Generic::IList<String^>^ pathList, 
			System::Collections::Generic::IList<String^>^ bootstrapList)
	{
		setvar("LUA_APP_TYPE",appType);	

		if(pathList!=nullptr) {
			System::Collections::Generic::IEnumerator<String^>^ it = pathList->GetEnumerator();
			while(it->MoveNext()) {
				addpath(it->Current);
			}
		}
		if(bootstrapList!=nullptr) {
			System::Collections::Generic::IEnumerator<String^>^ it = bootstrapList->GetEnumerator();
			while(it->MoveNext()) {
				require(it->Current);
			}
		}	
	}

	Object^ LuaApp::createObject(String^ type, LuaHostArray_Ref ps)
	{
		if(m_creators!=nullptr) {
			System::Collections::Generic::IEnumerator<LuaObjectCreator^>^ it = m_creators->GetEnumerator();
			while(it->MoveNext()) {
				Object^ o = it->Current->createObject(this,type,ps);
				if(o!=nullptr) {
					return o;
				}
			}
		}
		return nullptr;
	}

	void LuaApp::addObject(String^ id, Object^ obj)
	{
		if(m_objects==nullptr)
			m_objects = gcnew System::Collections::Generic::Dictionary<String^,Object^>();
		if(m_objects->ContainsKey(id)) 
		{
			m_objects->Remove(id);
		}
		m_objects->Add(id, obj);
	}

	Object^ LuaApp::getObject(String^ id)
	{
		if(m_objects!=nullptr) {
			if(m_objects->ContainsKey(id)) {
				return m_objects[id];
			}
		}
		return nullptr;
	}
		
	void LuaApp::removeObject(String^ id)
	{
		if(m_objects!=nullptr) {
			if(m_objects->ContainsKey(id)) {
				m_objects->Remove(id);
			}
		}
	}

	void LuaApp::addObjectCreator(LuaObjectCreator^ loc)
	{
		if(m_creators!=nullptr)
		{
			m_creators = gcnew System::Collections::Generic::List<LuaObjectCreator^>();
		}
		m_creators->Add(loc);
	}

	// LuaAppRealm
	String^ LuaAppRealm::lapi_load(String^ name)
	{
		if(m_loader==nullptr) {
			throw gcnew Exception("loader invalid");
		}
		return m_loader->load(this, name);
	}

	bool LuaAppRealm::lapi_invoke(String^ method, int callId, LuaHostArray_Ref params)
	{
		LuaCall^ call;
		if(!m_calls->TryGetValue(method,call)) {
			String^ msg = String::Format("invalid method '{0}'", method);
			return m_host->buildCallError(params, marshalString(msg).c_str());
		}		
		try {
			return call->call(this, callId, params);
		} catch(Exception^ err) {
			return m_host->buildCallError(params, marshalString(err->Message));
		}
	}

	int LuaAppRealm::lapi_doTimer(int timerId, int fixTime)
	{
		if(!isOpen()) {
			return -1;
		}
		LuaHostArray ps = gcnew System::Collections::Generic::List<LuaValue^>();
		ps->Add(LuaValue::intValue(timerId));
		ps->Add(LuaValue::intValue(fixTime));
		bool r = pcall(LUA_FUNCTION_HOST_TIMER_RESPONSE, ps, ps);
		if(r) {
#ifdef _DEBUG
			System::Console::WriteLine("[LuaHost] timer[{0}] => done - {1}", timerId, ps->Count>0?ps[0]->booleanValue():false);
#endif
		} else {
#ifdef _DEBUG
			System::Console::WriteLine("[LuaHost] timer[{0}] => fail - {1}", timerId, (ps->Count>0?ps[0]->stringValue():""));
#endif
		}
        if (r && fixTime > 0)
        {
			if(ps->Count>0 && ps[0]->booleanValue()) {				
				return fixTime;
			} else {
#ifdef _DEBUG
				System::Console::WriteLine("[LuaHost] timer[{0}] => stop timer", timerId);
#endif
			}
        }
		return -1;
	}

	bool LuaAppRealm::canCall()
	{
		return false;
	}
	bool LuaAppRealm::canInvoke(String^ name)
	{
		if(name!=nullptr) {
			if(name->Equals("createObject")) {
				return true;
			}
		}
		return false;
	}

	bool LuaAppRealm::call(LuaApp^ app, LuaHostArray_Ref ctx)
	{		
		ctx->Clear();
		return false;
	}
	
	bool LuaAppRealm::invoke(LuaApp^ app, String^ name, LuaHostArray_Ref ctx)
	{
		if(name!=nullptr) {
			if(name->Equals("createObject")) {
				String^ type = "";
				if(ctx->Count>0) {
					type = ctx[0]->stringValue();
					ctx->RemoveAt(0);
				}
				Object^ o = createObject(type,ctx);
				ctx->Clear();
				ctx->Add(LuaValue::objectValue(o));
				return true;
			}
		}
		ctx->Clear();
		ctx->Add(LuaValue::stringValue("not support '"+name+"'"));
		return false;
	}

	void LuaAppRealm::discard(LuaApp^ app)
	{

	}

	bool LuaAppRealm::canGetValue(String^ name)
	{
		if(name!=nullptr) {
			if(m_objects!=nullptr && m_objects->ContainsKey(name)) {
				return true;
			}
		}
		return false;
	}
	
	LuaValue^ LuaAppRealm::getValue(LuaApp^ app, String^ name)
	{
		if(name!=nullptr) {
			if(m_objects!=nullptr && m_objects->ContainsKey(name)) {
				Object^ o = m_objects[name];
				return LuaValue::objectValue(o);
			}
		}
		return LuaValue::nullValue();
	}
	

}
// luanet.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Reflection;
using namespace System::Collections;
using namespace System::Text;
using namespace System::Security;
using namespace System::Collections::Generic;

// #include <vcclr.h>
// #define _WINNT_
// #include <WinDef.h>
#include <vcclr.h>
// #include <atlstr.h>
#include <stdio.h>
#using <mscorlib.dll>
#include <string.h>

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

/*********************** LUA API FUNCTIONS ******************************/
static int lua_dotnetcall(lua_State *L) {
	lua_pushstring( L , "_dotnetcall");
	lua_rawget( L , LUA_REGISTRYINDEX );

	if( !lua_islightuserdata(L, -1))	
	{
		lua_pushstring( L , "invalid dotnetcall function" );
		lua_error( L );
	}	
	lua_CFunction f = (lua_CFunction) lua_touserdata(L , -1);
	lua_pop(L,1);
	return f(L);
}

static int lua_dotnetloader(lua_State *L) {
	lua_pushstring( L , "_dotnetloader");
	lua_rawget( L , LUA_REGISTRYINDEX );

	if( !lua_islightuserdata(L, -1))	
	{
		lua_pushstring( L , "invalid dotnetloader function" );
		return 1;
	}	
	lua_CFunction f = (lua_CFunction) lua_touserdata(L , -1);
	lua_pop(L,1);
	return f(L);
}

static struct luaL_Reg dotnetlib[] = {
  {"call", lua_dotnetcall},
  {NULL, NULL}
};


int luaopen_dotnet_ext (lua_State *L) {
  luaL_openlib(L, "dotnet", dotnetlib, 0);  
  return 1;
}

#define toState		((lua_State *) luaState.ToPointer())

// Not sure of the purpose of this, but I'm keeping it -kevinh
static int tag = 0;

namespace Lua
{
	template<class BaseType>
	bool is(Type^ type)
	{
		return BaseType::typeid->IsAssignableFrom(type);
	}

	template<class ToType, class FromType> 
	ToType as(FromType u)
	{
		ToType t;
		try{
			t = (ToType)u;
		}
		catch(System::Exception^){
			t = nullptr;
		}
		return t;
	}

	/*
	 * Delegate for functions passed to Lua as function pointers
	 */
	ref class LuaState;
	public delegate int LuaCFunction(IntPtr L);
	public delegate bool LuaCallback(LuaState^ luaState, List<Object^>^ params);
	public delegate String^ LuaLoader(LuaState^ luaState, String^ name);

	// To fix the strings:
	// http://support.microsoft.com/kb/311259

	public ref class LuaState
	{
	private:
		 static Encoding^ u8 = Encoding::UTF8;
		 IntPtr luaState;
		 LuaCFunction^ csCall;
		 LuaCFunction^ csLoader;
		 LuaCallback^ dotnetCall;
		 LuaLoader^ dotnetLoader;

	private:
		static void pushDotnetValue(lua_State* L,Object^ v) {
			if(v==nullptr) {
				lua_pushnil(L);
				return;
			}
			Type^ t = v->GetType();
			if(t==double::typeid) {
				double^ o = as<double^>(v);
				if(o!=nullptr) {
					lua_pushnumber(L, (double) o);
				} else {
					lua_pushnumber(L, 0);					
				}				
				return;
			}
			if(is<int^>(t)) {
				int^ o = as<int^>(v);
				if(o!=nullptr) {
					lua_pushinteger(L, (int) o);
				} else {
					lua_pushinteger(L, 0);
				}				
				return;
			}			
			if(t==bool::typeid) {
				bool^ o = as<bool^>(v);
				if(o!=nullptr) {
					lua_pushboolean(L, ((bool) o)?1:0);					
				} else {
					lua_pushboolean(L, 0);
				}				
				return;
			}			
			if(is<List<Object^>>(t)) {
				List<Object^>^ o = as<List<Object^>^>(v);
				if(o!=nullptr) {
					int sz = o->Count;
					lua_newtable(L);
					for(int i=0;i<sz;i++) {
						Object^ v2 = o->default[i];	
						lua_pushnumber(L, i+1);
						pushDotnetValue(L, v2);
						lua_settable(L,-3);
					}	
					return;
				}
			}
			if(is<Dictionary<String^, Object^>^>(t)) {
				Dictionary<String^, Object^>^ o = as<Dictionary<String^, Object^>^>(v);
				if(o!=nullptr) {
					lua_newtable(L);
					for each(KeyValuePair<String^, Object^>^ pair in o)
					{
						String^ key = pair->Key;
						Object^ v2 = pair->Value;

						array<Byte>^bkey = u8->GetBytes(key);
						pin_ptr<Byte> pkey = &bkey[0];
					
						lua_pushstring(L, (const char*) pkey);					
						pushDotnetValue(L, v2);
						lua_settable(L,-3);
					}
					return;
				}
			}
			String^ str = v->ToString();
			array<Byte>^bstr = u8->GetBytes(str);
			pin_ptr<Byte> pstr = &bstr[0];
			lua_pushstring(L, (const char*) pstr);
		}

		static int pushStackData(lua_State* L, List<Object^>^ params) {
			int sz = params->Count;
			int i;	 
			for(i=0;i<sz;i++) {
				Object^ v = params->default[i];				
				pushDotnetValue(L, v);
			}	
			return sz;
		}

		static Object^ popLuaValue(lua_State* L, int idx) {
			int type = lua_type(L,idx);				
			switch(type) {
			case LUA_TBOOLEAN: {
				return (bool) (lua_toboolean(L,idx)!=0);
			}
			case LUA_TNUMBER: {
				lua_Integer v1 = lua_tointeger(L,idx);
				lua_Number v2 = lua_tonumber(L,idx);
				if(v1==v2) {
					return (int) v1;
				} else {
					return (double) v2;
				}				
			}
			case LUA_TSTRING: {
				const char* v = lua_tostring(L, idx);
				return gcnew String(v, 0, strlen(v), Encoding::UTF8);
			}		
			case LUA_TTABLE: {				
				if(idx<0) {
					idx = lua_gettop(L)+idx+1;
				}
				int len = isLuaArray(L, idx);
				if (len > 0) {
					List<Object^>^ r = gcnew List<Object^>();
					for (int i = 1; i <= len; i++) {		
						lua_rawgeti(L, idx, i);
						Object^ v = popLuaValue(L, -1);
						r->Add(v);
						lua_pop(L, 1);
					}
					return r;
				} else {
					Dictionary<String^, Object^>^ r = gcnew Dictionary<String^, Object^>();
					lua_pushnil(L);
					while (lua_next(L, idx) != 0) {
						Object^ key = popLuaValue(L, -2);
						Object^ v = popLuaValue(L,-1);
						if(key!=nullptr) {
							r->Add(key->ToString(), v);
						}
						lua_pop(L, 1);
					}
					return r;
				}
			}
			case LUA_TNIL:
			default:		
				return nullptr;
			}
		}

		static void popStackData(lua_State* L, List<Object^>^ data,int top,int nresults) {
			data->Clear();
			for(int i=1;i<=nresults;i++) {		
				data->Add(popLuaValue(L,top+i));
			}
			if(nresults>0) {
				lua_pop(L,nresults);
			}
		}

		int thisLuaCall(IntPtr nouse) {
			lua_State* L = toState;
			List<Object^>^ params = gcnew List<Object^>();
			popStackData(L, params,0,lua_gettop(L));
			try{
				if(dotnetCall==nullptr) {
					return luaL_error(L,"call is null");	
				}
				bool done = dotnetCall(this, params);
				int r = pushStackData(L, params);
				if(done)return r;
				return lua_error(L);
			} catch(System::Exception^ err){
				array<Byte>^bbuf = u8->GetBytes(err->Message);
				pin_ptr<Byte> pbuf = &bbuf[0];
				return luaL_error(L,(const char*) pbuf);
			}
		}

		int thisLuaLoader(IntPtr nouse) {
			lua_State* L = toState;
			if(dotnetLoader==nullptr) {
				lua_pushstring(L, "invalid .net loader");
				return 1;
			}
			const char* lname = lua_tostring(L,-1);
			String^ name = gcnew String(lname, 0, strlen(lname), Encoding::UTF8);			
			try{
				String^ content = dotnetLoader(this, name);
				if(content==nullptr) {
					lua_pushstring(L, ".net load fail");
					return 1;
				}
				array<Byte>^bcontent = u8->GetBytes(content);
				pin_ptr<Byte> pcontent = &bcontent[0];
				luaL_loadbuffer(L, (const char*) pcontent, bcontent->Length, lname);				
				return 1;				
			} catch(System::Exception^ err){
				array<Byte>^bbuf = u8->GetBytes(err->Message);
				pin_ptr<Byte> pbuf = &bbuf[0];
				lua_pushstring(L, (const char*) pbuf);
				return 1;
			}
		}

	public:
		bool open()
		{
			lua_State* L = ::luaL_newstate();
			::luaL_openlibs(L);  /* open libraries */
			::luamodule_cjson(L);
			::luamodule_cjson_safe(L);
			::luamodule_md5(L);
			::luamodule_des56(L);
			::luaopen_dotnet_ext(L);

			luaState = IntPtr(L);
			csCall = gcnew LuaCFunction(this, &LuaState::thisLuaCall); 

			IntPtr p = Marshal::GetFunctionPointerForDelegate(csCall);
			lua_pushstring( L , "_dotnetcall");
			lua_pushlightuserdata(L, p.ToPointer());
			lua_settable( L , LUA_REGISTRYINDEX );		

			return true;
		}

		void close()
		{
			lua_State* L = toState;
			if(L!=NULL) {
				::lua_close(L);
				luaState = IntPtr(nullptr);
				csCall = nullptr;
				csLoader = nullptr;
			}
		}

		void addpath(String^ path)
		{
			lua_State* L = toState;
			if(L==NULL)return;
			array<Byte>^bbuf = u8->GetBytes(path);
			pin_ptr<Byte> pbuf = &bbuf[0];
			::addSearchPath( toState , (const char*) pbuf);
		}

		void setvar(String^ key, String^ value)
		{
			lua_State* L = toState;
			if(L==NULL)return;

			array<Byte>^bkey = u8->GetBytes(key);
			pin_ptr<Byte> pkey = &bkey[0];

			array<Byte>^bvalue = u8->GetBytes(value);
			pin_ptr<Byte> pvalue = &bvalue[0];

			::lua_pushstring(L, (const char*) pvalue);
			::lua_setfield(L, LUA_GLOBALSINDEX, (const char*) pkey);
		}

		bool pcall(String^ fun, List<Object^>^ params)
		{
			lua_State* L = toState;
			if(L==NULL) {
				params->Clear();
				params->Add(gcnew String("luaState not open"));
				return false;
			}

			int top = lua_gettop(L);

			array<Byte>^bfun = u8->GetBytes(fun);
			pin_ptr<Byte> pfun = &bfun[0];

			/* function to be called */
			lua_getfield(L, LUA_GLOBALSINDEX, (const char*) pfun);
			int sz = pushStackData(L, params);
			int err = lua_pcall(L, sz, LUA_MULTRET, 0);
			sz = lua_gettop(L) - top;
			popStackData(L, params, top, sz);
			return err==0;
		}

		String^ eval(String^ s)
		{
			lua_State* L = toState;
			if(L==NULL) {
				return gcnew String("luaState not open");
			}

			array<Byte>^bbuf = u8->GetBytes(s);
			pin_ptr<Byte> pbuf = &bbuf[0];
			int err = luaL_dostring(L, (const char*) pbuf);
			if(err!=0) {
				const char* s = lua_tostring(L,-1);
				String^ r = gcnew String(s, 0, strlen(s), Encoding::UTF8);
				lua_pop(L,1);
				return r;
			}
			return nullptr;
		}

		void callback(LuaCallback^ function)
		{
			dotnetCall = function;					
		}

		void loader(LuaLoader^ l)
		{
			dotnetLoader = l;
			if(csLoader==nullptr) {
				lua_State* L = toState;

				csLoader = gcnew LuaCFunction(this, &LuaState::thisLuaLoader); 

				IntPtr p = Marshal::GetFunctionPointerForDelegate(csLoader);
				lua_pushstring( L , "_dotnetloader");
				lua_pushlightuserdata(L, p.ToPointer());
				lua_settable( L , LUA_REGISTRYINDEX );		

				setModuleLoader(toState, lua_dotnetloader, 1);
			}
		}
	};
}



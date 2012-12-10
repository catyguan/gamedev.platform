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

static struct luaL_Reg dotnetlib[] = {
  {"call", lua_dotnetcall},
  {NULL, NULL}
};


int luaopen_dotnet_ext (lua_State *L) {
  luaL_openlib(L, "dotnet", dotnetlib, 0);  
  return 1;
}

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
	public delegate int LuaCSFunction(IntPtr luaState);

	// To fix the strings:
	// http://support.microsoft.com/kb/311259

	public ref class LuaAPI
	{
	private:
		 static Encoding^ u8 = Encoding::UTF8;

	public:

#define toState		((lua_State *) luaState.ToPointer())

		static IntPtr open()
		{
			lua_State* L = ::luaL_newstate();
			::luaL_openlibs(L);  /* open libraries */
			::luamodule_cjson(L);
			::luamodule_cjson_safe(L);
			::luamodule_md5(L);
			::luamodule_des56(L);
			::luaopen_dotnet_ext(L);
			return IntPtr(L);
		}

		static void error(IntPtr luaState)
		{
			::lua_error(toState);
		}

		static void close(IntPtr luaState)
		{
			::lua_close(toState);
		}

		static void addpath(IntPtr luaState, String^ path)
		{
			array<Byte>^bbuf = u8->GetBytes(path);
			pin_ptr<Byte> pbuf = &bbuf[0];
			::addSearchPath( toState , (const char*) pbuf);
		}

		static void setvar(IntPtr luaState, String^ key, String^ value)
		{
			lua_State* L = toState;

			array<Byte>^bkey = u8->GetBytes(key);
			pin_ptr<Byte> pkey = &bkey[0];

			array<Byte>^bvalue = u8->GetBytes(value);
			pin_ptr<Byte> pvalue = &bvalue[0];

			::lua_pushstring(L, (const char*) pvalue);
			::lua_setfield(L, LUA_GLOBALSINDEX, (const char*) pkey);
		}

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

		static void getParams(IntPtr luaState, List<Object^>^ data,int top,int nresults) {
			popStackData(toState,data,top,nresults);
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

		static bool pcall(IntPtr luaState, String^ fun, List<Object^>^ params)
		{
			lua_State* L = toState;
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

		static String^ eval(IntPtr luaState, String^ s)
		{
			lua_State* L = toState;

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

		static int gettop(IntPtr luaState) {
			return lua_gettop(toState);
		}

		static void callfunction(IntPtr luaState, LuaCSFunction^ function)
		{
			lua_State* L = toState;
			IntPtr p = Marshal::GetFunctionPointerForDelegate(function);
			lua_pushstring( L , "_dotnetcall");
			lua_pushlightuserdata(L, p.ToPointer());
			lua_settable( L , LUA_REGISTRYINDEX );			
		}
	};
}

// luarunner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern "C" {

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

// JSON
void luamodule_cjson(lua_State *L);  
void luamodule_cjson_safe(lua_State *L);
// MD5
void luamodule_md5(lua_State *L);
// DES56
void luamodule_des56(lua_State *L);

int luamain (lua_State *L, int argc, char **argv);

}

#include <string>
#include "../../gamedev.mgr/MGRunner/Classes/SimpleIni.h"

static void addSearchPath(lua_State* L, const char* path)
{
	const char* cur_path;
    lua_getglobal(L, "package");                              /* stack: package */
    lua_getfield(L, -1, "path");            /* get package.path, stack: package path */
    cur_path =  lua_tostring(L, -1);
    lua_pop(L, 1);                                            /* stack: package */
    lua_pushfstring(L, "%s;%s/?.lua", cur_path, path);        /* stack: package newpath */
    lua_setfield(L, -2, "path");      /* package.path = newpath, stack: package */
	lua_pop(L, 1);                                            /* stack: - */
}

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State *L = lua_open();  /* create state */
	if (L != NULL) {		

		luaL_openlibs(L);  /* open libraries */
		luamodule_cjson(L);
		luamodule_cjson_safe(L);
		luamodule_md5(L);
		luamodule_des56(L);

		CSimpleIniA ini;
		ini.LoadFile("app.ini");

		const char* tmp;
		tmp = ini.GetValue("Application","libs.dir",NULL);
		if(tmp!=NULL) {
			std::string s(tmp);
			size_t last = 0;
			size_t index=s.find_first_of(";",last);
			while (index!=std::string::npos)
			{
				addSearchPath(L, s.substr(last,index-last).c_str());
				last=index+1;
				index=s.find_first_of(";",last);
			}
			if (index-last>0)
			{
				addSearchPath(L, s.substr(last,index-last).c_str());
			}			
		}
	}
	return luamain(L, argc,argv);
}

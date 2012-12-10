#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int isLuaArray(lua_State *l, int idx)
{
    double k;
    int max;
    int items;

    max = 0;
    items = 0;

    lua_pushnil(l);
    /* table, startkey */
	while (lua_next(l, idx) != 0) {
        /* table, key, value */
		if (lua_type(l, -2) == LUA_TNUMBER &&
			(k = lua_tonumber(l, -2))) {
            /* Integer >= 1 ? */
            if (floor(k) == k && k >= 1) {
                if (k > max)
                    max = k;
                items++;
                lua_pop(l, 1);
                continue;
            }
        }

        /* Must not be an array (non integer key) */
        lua_pop(l, 2);
        return -1;
    }

    return max;
}

void addSearchPath(lua_State* L, const char* path)
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

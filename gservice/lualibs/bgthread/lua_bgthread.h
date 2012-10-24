#ifndef LUA_BGTHREAD_H
#define LUA_BGTHREAD_H

#include <lua.h>

typedef const char* (*log_luaerror) (lua_State *L);

// 0, ok; -1, fail
int bgthread_start(lua_State* state,log_luaerror logger);

void bgthread_initcall(const char* content);

void bgthread_stop();

#endif
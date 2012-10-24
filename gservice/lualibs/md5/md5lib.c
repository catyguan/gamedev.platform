/**
*  $Id: md5lib.c,v 1.10 2008/05/12 20:51:27 carregal Exp $
*  Cryptographic and Hash functions for Lua
*  @author  Roberto Ierusalimschy
*/


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>

#if ! defined (LUA_VERSION_NUM) || LUA_VERSION_NUM < 501
#include "compat-5.1.h"
#endif

#include "md5.h"


/**
*  Hash function. Returns a hash for a given string.
*  @param message: arbitrary binary string.
*  @return  A 128-bit hash string.
*/
static int lmd5 (lua_State *L) {
  char buff[16];
  size_t l;
  const char *message = luaL_checklstring(L, 1, &l);
  md5(message, l, buff);
  lua_pushlstring(L, buff, 16L);
  return 1;
}

static void checkseed (lua_State *L) {
  if (lua_isnone(L, 3)) {  /* no seed? */
    time_t tm = time(NULL);  /* for `random' seed */
    lua_pushlstring(L, (char *)&tm, sizeof(tm));
  }
}


#define MAXKEY	256
#define BLOCKSIZE	16



static int initblock (lua_State *L, const char *seed, int lseed, char *block) {
  size_t lkey;
  const char *key = luaL_checklstring(L, 2, &lkey);
  if (lkey > MAXKEY)
    luaL_error(L, "key too long (> %d)", MAXKEY);
  memset(block, 0, BLOCKSIZE);
  memcpy(block, seed, lseed);
  memcpy(block+BLOCKSIZE, key, lkey);
  return (int)lkey+BLOCKSIZE;
}


static void codestream (lua_State *L, const char *msg, size_t lmsg,
                                      char *block, int lblock) {
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while (lmsg > 0) {
    char code[BLOCKSIZE];
    int i;
    md5(block, lblock, code);
    for (i=0; i<BLOCKSIZE && lmsg > 0; i++, lmsg--)
      code[i] ^= *msg++;
    luaL_addlstring(&b, code, i); 
    memcpy(block, code, i); /* update seed */
  }
  luaL_pushresult(&b);
}


static void decodestream (lua_State *L, const char *cypher, size_t lcypher,
                          char *block, int lblock) {
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while (lcypher > 0) {
    char code[BLOCKSIZE];
    int i;
    md5(block, lblock, code);  /* update seed */
    for (i=0; i<BLOCKSIZE && lcypher > 0; i++, lcypher--)
      code[i] ^= *cypher++;
    luaL_addlstring(&b, code, i); 
    memcpy(block, cypher-i, i);
  }
  luaL_pushresult(&b);
}

/*
** Assumes the table is on top of the stack.
*/
static void set_info (lua_State *L) {
	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, "Copyright (C) 2003 PUC-Rio");
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, "Basic cryptographic facilities");
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, "MD5 1.1.2");
	lua_settable (L, -3);
}


static struct luaL_Reg md5lib[] = {
  {"sum", lmd5},
  {NULL, NULL}
};


int luaopen_md5_core (lua_State *L) {
  luaL_openlib(L, "md5", md5lib, 0);
  set_info (L);
  return 1;
}

void luamodule_md5(lua_State *L) {
	luaopen_md5_core(L);
}


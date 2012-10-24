#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "luajni.h"
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

/* Constant that is used to index the JNI Environment */
#define LUAJAVA_JNIENV		"__JAVA_JNIEnv"
/* Defines the lua State Index Property Name */
#define LUAJAVA_STATEID		"__JAVA_StateId"

static lua_State * getStateFromJObj( JNIEnv * env , jlong cptr );

static void pushJNIEnv( JNIEnv * env , lua_State * L );

static JNIEnv * getEnvFromState( lua_State * L );

static jclass    cls_LuaState				= NULL;
static jmethodID mid_LuaState__luacall		= NULL;

static jclass    cls_LuaStackData			= NULL;
static jmethodID mid_LuaStackData_new		= NULL;
static jmethodID mid_LuaStackData_getSize	= NULL;
static jmethodID mid_LuaStackData_getType	= NULL;
static jmethodID mid_LuaStackData_getInt	= NULL;
static jmethodID mid_LuaStackData_getNumber	= NULL;
static jmethodID mid_LuaStackData_getString	= NULL;
static jmethodID mid_LuaStackData_getBoolean	= NULL;
static jmethodID mid_LuaStackData_reset		= NULL;
static jmethodID mid_LuaStackData_addNull	= NULL;
static jmethodID mid_LuaStackData_addInt	= NULL;
static jmethodID mid_LuaStackData_addNumber	= NULL;
static jmethodID mid_LuaStackData_addString	= NULL;
static jmethodID mid_LuaStackData_addBoolean= NULL;

// static jclass	 cls_Exception				= NULL;
// static jmethodID mid_Exception__getMessage	= NULL;

void initJNICache(JNIEnv* env,int cb)
{
	if(cb) {
		cls_LuaState = ( *env )->FindClass( env , "ge/lua/LuaState" );	
		mid_LuaState__luacall = ( *env )->GetStaticMethodID( env , cls_LuaState, "_luacallback" , "(ILge/lua/LuaStackData;)Z" );
	}

	cls_LuaStackData = ( *env )->FindClass( env , "ge/lua/LuaStackData" );	
	if(cb) {
		mid_LuaStackData_new = ( *env )->GetMethodID( env , cls_LuaStackData, "<init>", "()V" );
	}
	mid_LuaStackData_getSize = ( *env )->GetMethodID( env , cls_LuaStackData, "getSize" , "()I" );
	mid_LuaStackData_getType = ( *env )->GetMethodID( env , cls_LuaStackData, "getType" , "(I)I" );
	mid_LuaStackData_getInt = ( *env )->GetMethodID( env , cls_LuaStackData, "getInt" , "(I)I" );
	mid_LuaStackData_getNumber = ( *env )->GetMethodID( env , cls_LuaStackData, "getNumber" , "(I)D" );
	mid_LuaStackData_getString = ( *env )->GetMethodID( env , cls_LuaStackData, "getString" , "(I)Ljava/lang/String;" );
	mid_LuaStackData_getBoolean = ( *env )->GetMethodID( env , cls_LuaStackData, "getBoolean" , "(I)Z" );
	mid_LuaStackData_reset = ( *env )->GetMethodID( env , cls_LuaStackData, "reset" , "()V" );
	mid_LuaStackData_addNull = ( *env )->GetMethodID( env , cls_LuaStackData, "addNull" , "()V" );
	mid_LuaStackData_addInt = ( *env )->GetMethodID( env , cls_LuaStackData, "addInt" , "(I)V" );
	mid_LuaStackData_addNumber = ( *env )->GetMethodID( env , cls_LuaStackData, "addNumber" , "(D)V" );
	mid_LuaStackData_addString = ( *env )->GetMethodID( env , cls_LuaStackData, "addString" , "(Ljava/lang/String;)V" );
	mid_LuaStackData_addBoolean = ( *env )->GetMethodID( env , cls_LuaStackData, "addBoolean" , "(Z)V" );

	// cls_Exception = ( *env )->FindClass( env , "java/lang/Exception" );
	// mid_Exception__getMessage = ( *env )->GetMethodID( env , cls_Exception, "getMessage" , "()Ljava/lang/String;" );
}

int getLuaStateId(lua_State* L) {
	int r = 0;

	lua_pushstring( L , LUAJAVA_STATEID );
	lua_rawget( L , LUA_REGISTRYINDEX );

	if ( !lua_isnumber( L , -1 ) )
	{
		lua_pushstring( L , "Impossible to identify luaState id." );
		lua_error( L );
	}
	
	r = lua_tointeger( L , -1 );
	lua_pop(L,1);
	return r;
}

/***************************************************************************
*
*  Function: getStateFromJObj
*  ****/

lua_State * getStateFromJObj( JNIEnv * env , jlong cptr )
{
	return ( lua_State * ) cptr;
}

/***************************************************************************
*
*  Function: getEnvFromState
*  ****/

JNIEnv * getEnvFromState( lua_State * L )
{
	JNIEnv ** udEnv;

	lua_pushstring( L , LUAJAVA_JNIENV );
	lua_rawget( L , LUA_REGISTRYINDEX );

	if ( !lua_isuserdata( L , -1 ) )
	{
		lua_pop( L , 1 );
		return NULL;
	}

	udEnv = ( JNIEnv ** ) lua_touserdata( L , -1 );

	lua_pop( L , 1 );

	return * udEnv;
}


/***************************************************************************
*
*  Function: pushJNIEnv
*  ****/

void pushJNIEnv( JNIEnv * env , lua_State * L )
{
	JNIEnv ** udEnv;

	lua_pushstring( L , LUAJAVA_JNIENV );
	lua_rawget( L , LUA_REGISTRYINDEX );

	if ( !lua_isnil( L , -1 ) )
	{
		udEnv = ( JNIEnv ** ) lua_touserdata( L , -1 );
		*udEnv = env;
		lua_pop( L , 1 );
	}
	else
	{
		lua_pop( L , 1 );
		udEnv = ( JNIEnv ** ) lua_newuserdata( L , sizeof( JNIEnv * ) );
		*udEnv = env;

		lua_pushstring( L , LUAJAVA_JNIENV );
		lua_insert( L , -2 );
		lua_rawset( L , LUA_REGISTRYINDEX );
	}
}

static void popStackData(lua_State* L,JNIEnv* env, jobject data,int top,int nresults) {
	int i;
	( *env )->CallVoidMethod(env,data,mid_LuaStackData_reset);
	for(i=1;i<=nresults;i++) {
		int type = lua_type(L,top+i);				
		switch(type) {
		case LUA_TBOOLEAN:
			( *env )->CallVoidMethod(env,data,mid_LuaStackData_addBoolean,lua_toboolean(L,top+i));
			break;
		case LUA_TNUMBER: {
				lua_Integer v = lua_tointeger(L,top+i);
				lua_Number v2 = lua_tonumber(L,top+i);
				if(v==v2) {
					( *env )->CallVoidMethod(env,data,mid_LuaStackData_addInt,v);
				} else {
					( *env )->CallVoidMethod(env,data,mid_LuaStackData_addNumber,v2);
				}				
				break;
			}
		case LUA_TSTRING: {
				const char* v = lua_tostring(L,top+i);
				jstring jstr = ( *env )->NewStringUTF(env,v);
				( *env )->CallVoidMethod(env,data,mid_LuaStackData_addString,jstr);
				( *env )->DeleteLocalRef(env, jstr);
				break;
			}			
		case LUA_TNIL:
		default:
			( *env )->CallVoidMethod(env,data,mid_LuaStackData_addNull);
			break;
		}
	}
	if(nresults>0) {
		lua_pop(L,nresults);
	}
}

static int pushStackData(lua_State* L,JNIEnv* env, jobject data) {
	int sz = ( *env )->CallIntMethod(env,data,mid_LuaStackData_getSize);
	int i;	 
	for(i=0;i<sz;i++) {
		int type = ( *env )->CallIntMethod(env,data,mid_LuaStackData_getType,i);
		switch(type) {
			case -2: {
					lua_Integer v = ( *env )->CallIntMethod(env,data,mid_LuaStackData_getInt,i);
					lua_pushinteger(L, v);
					break;
				}			
			case LUA_TBOOLEAN: {
					jboolean v = ( *env )->CallBooleanMethod(env,data,mid_LuaStackData_getBoolean,i);
					lua_pushboolean(L,v);
					break;
				}
			case LUA_TNUMBER: {
					jdouble v = ( *env )->CallDoubleMethod(env,data,mid_LuaStackData_getNumber,i);
					lua_pushnumber(L,v);
					break;
				}
			case LUA_TSTRING: {
					jstring ret;
					ret = (jstring) ( *env )->CallObjectMethod( env,data,mid_LuaStackData_getString,i);
					if(ret!=NULL) {
						const char* c = ( *env )->GetStringUTFChars( env, ret, NULL );
						lua_pushstring(L, c);
						( *env )->ReleaseStringUTFChars( env , ret, c );
					} else {
						lua_pushnil(L);
					}
					break;
				}
			case LUA_TNIL:
			default:{
					lua_pushnil(L);
					break;
				}
		}		
	}	
	return sz;
}

/*********************** LUA API FUNCTIONS ******************************/
static int lua_javacall(lua_State *L) {		
	int stateId = getLuaStateId(L);	
	JNIEnv * env = getEnvFromState( L );	
	int n = lua_gettop(L);  /* number of arguments */	
	jobject obj;
	jboolean ret;

	initJNICache(env, 1);
	obj = (*env)->NewObject(env, cls_LuaStackData, mid_LuaStackData_new);
	popStackData(L,env,obj,0, n);
	ret = ( *env )->CallStaticBooleanMethod( env , cls_LuaState, mid_LuaState__luacall , 
			stateId , obj);
	n = pushStackData(L,env,obj);
	( *env )->DeleteLocalRef(env, obj);
	if(ret) {		
		return n;
	} else {
		return lua_error(L);
	}
}

static struct luaL_Reg javalib[] = {
  {"call", lua_javacall},
  {NULL, NULL}
};


int luaopen_java_ext (lua_State *L) {
  luaL_openlib(L, "java", javalib, 0);  
  return 1;
}

/************************************************************************
*   JNI Called function
************************************************************************/

JNIEXPORT jlong JNICALL Java_ge_lua_LuaState__1open
  (JNIEnv * env , jobject jobj, jint stateId)
{
	lua_State * L = NULL;

	L = luaL_newstate();

	luaL_openlibs(L);  /* open libraries */
	luamodule_cjson(L);
	luamodule_cjson_safe(L);
	luamodule_md5(L);
	luamodule_des56(L);
	luaopen_java_ext(L);
 
	lua_pushstring( L , LUAJAVA_STATEID );
	lua_pushnumber( L , (lua_Number)stateId );
	lua_settable( L , LUA_REGISTRYINDEX );

	pushJNIEnv(env, L);   

	return (jlong) L;
}

JNIEXPORT void JNICALL Java_ge_lua_LuaState__1threadInit
  (JNIEnv * env , jobject jobj, jlong cptr)
{
	lua_State * L = getStateFromJObj( env , cptr );
	pushJNIEnv(env, L);	
	
}

JNIEXPORT void JNICALL Java_ge_lua_LuaState__1close
  (JNIEnv * env , jobject jobj , jlong cptr)
{
	lua_State * L = getStateFromJObj( env , cptr );
	lua_close( L );
}

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

JNIEXPORT jboolean JNICALL Java_ge_lua_LuaState__1addpath
   (JNIEnv * env, jobject jThis, jlong data, jstring path) 
{	
	lua_State * L = getStateFromJObj( env , data );
	const char* k = ( *env )->GetStringUTFChars( env, path , NULL );
	addSearchPath( L , k);
	( *env )->ReleaseStringUTFChars( env , path , k );
	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_ge_lua_LuaState__1setvar
   (JNIEnv * env, jobject jThis, jlong data, jstring key, jstring value) 
{	
	lua_State * L = getStateFromJObj( env , data );
	const char* k = ( *env )->GetStringUTFChars( env, key, NULL );
	const char* v = ( *env )->GetStringUTFChars( env, value, NULL );
	lua_pushstring(L, v);
	lua_setfield(L, LUA_GLOBALSINDEX, k);
	( *env )->ReleaseStringUTFChars( env , key , k );
	( *env )->ReleaseStringUTFChars( env , value, v );
	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_ge_lua_LuaState__1pcall
  (JNIEnv * env, jobject jThis, jlong data, jstring fun, jobject reqp)
{
	lua_State * L = getStateFromJObj( env , data );	
	int top = lua_gettop(L);
	const char* f = ( *env )->GetStringUTFChars( env, fun, NULL );
	int sz,err;
	
	initJNICache(env, 0);

	lua_getfield(L, LUA_GLOBALSINDEX, f); /* function to be called */
	sz = pushStackData(L, env, reqp);	
    err = lua_pcall(L, sz, LUA_MULTRET, 0);	  /* call 'f' with 'sz' arguments and 'MULTRET' result */	
	sz = lua_gettop(L) - top;
	( *env )->ReleaseStringUTFChars( env , fun , f );
	popStackData(L,env,reqp,top,sz);

	return err==0;
}

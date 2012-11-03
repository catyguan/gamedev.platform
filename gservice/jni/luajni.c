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

#define JC_LuaStackData	0
#define JCLASS_MAX	1

#define JM_LuaStackData_new			0
#define JM_LuaStackData_getSize		1
#define JM_LuaStackData_getType		2
#define JM_LuaStackData_getInt		3
#define JM_LuaStackData_getNumber	4
#define JM_LuaStackData_getString	5
#define JM_LuaStackData_getBoolean	6
#define JM_LuaStackData_reset		7
#define JM_LuaStackData_addNull		8
#define JM_LuaStackData_addInt		9
#define JM_LuaStackData_addNumber	10
#define JM_LuaStackData_addString	11
#define JM_LuaStackData_addBoolean	12
#define JMETHOD_MAX	13

typedef struct {
	jclass c[JCLASS_MAX];
	jmethodID m[JMETHOD_MAX];
} JAPI;

static jclass jc(JNIEnv* env,JAPI* api,int id) {
	if(!api->c[id]) {
		jclass c;
		switch(id) {
		case JC_LuaStackData:
			c = ( *env )->FindClass( env , "ge/lua/LuaStackData" );
			break;
		}
		api->c[id] = c;
	}
	return api->c[id];
}

static jmethodID jm(JNIEnv* env, JAPI* api,int id) {
	if(!api->m[id]) {
		jmethodID m;
		switch(id) {
		case JM_LuaStackData_new:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "<init>", "()V" );
			break;
		case JM_LuaStackData_getSize:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "getSize" , "()I" );	
			break;
		case JM_LuaStackData_getType:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "getType" , "(I)I" );	
			break;
		case JM_LuaStackData_getInt:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "getInt" , "(I)I" );	
			break;
		case JM_LuaStackData_getNumber:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "getNumber" , "(I)D" );	
			break;
		case JM_LuaStackData_getString:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "getString" , "(I)Ljava/lang/String;" );	
			break;
		case JM_LuaStackData_getBoolean:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "getBoolean" , "(I)Z" );
			break;
		case JM_LuaStackData_reset:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "reset" , "()V" );
			break;
		case JM_LuaStackData_addNull:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "addNull" , "()V" );
			break;
		case JM_LuaStackData_addInt:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "addInt" , "(I)V" );	
			break;
		case JM_LuaStackData_addNumber:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "addNumber" , "(D)V" );	
			break;
		case JM_LuaStackData_addString:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "addString" , "(Ljava/lang/String;)V" );	
			break;
		case JM_LuaStackData_addBoolean:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaStackData), "addBoolean" , "(Z)V" );
			break;
		}
		api->m[id] = m;
	}
	return api->m[id];
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

static void popStackData(lua_State* L,JNIEnv* env, JAPI* api, jobject data,int top,int nresults) {
	int i;
	( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaStackData_reset));
	for(i=1;i<=nresults;i++) {
		int type = lua_type(L,top+i);				
		switch(type) {
		case LUA_TBOOLEAN:
			( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaStackData_addBoolean),lua_toboolean(L,top+i));
			break;
		case LUA_TNUMBER: {
				lua_Integer v = lua_tointeger(L,top+i);
				lua_Number v2 = lua_tonumber(L,top+i);
				if(v==v2) {
					( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaStackData_addInt),v);
				} else {
					( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaStackData_addNumber),v2);
				}				
				break;
			}
		case LUA_TSTRING: {
				const char* v = lua_tostring(L,top+i);
				jstring jstr = ( *env )->NewStringUTF(env,v);
				( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaStackData_addString),jstr);
				( *env )->DeleteLocalRef(env, jstr);
				break;
			}			
		case LUA_TNIL:
		default:
			( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaStackData_addNull));
			break;
		}
	}
	if(nresults>0) {
		lua_pop(L,nresults);
	}
}

static int pushStackData(lua_State* L,JNIEnv* env, JAPI* api, jobject data) {
	int sz = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaStackData_getSize));
	int i;	 
	for(i=0;i<sz;i++) {
		int type = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaStackData_getType),i);
		switch(type) {
			case -2: {
					lua_Integer v = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaStackData_getInt),i);
					lua_pushinteger(L, v);
					break;
				}			
			case LUA_TBOOLEAN: {
					jboolean v = ( *env )->CallBooleanMethod(env,data,jm(env,api,JM_LuaStackData_getBoolean),i);
					lua_pushboolean(L,v);
					break;
				}
			case LUA_TNUMBER: {
					jdouble v = ( *env )->CallDoubleMethod(env,data,jm(env,api,JM_LuaStackData_getNumber),i);
					lua_pushnumber(L,v);
					break;
				}
			case LUA_TSTRING: {
					jstring ret;
					ret = (jstring) ( *env )->CallObjectMethod( env,data,jm(env,api,JM_LuaStackData_getString),i);
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
	jclass    cls_LuaState = ( *env )->FindClass( env , "ge/lua/LuaState" );	
	jmethodID mid_LuaState__luacall	= ( *env )->GetStaticMethodID( env , cls_LuaState, "_luacallback" , "(ILge/lua/LuaStackData;)Z" );
	JAPI api;

	memset(&api,0,sizeof(api));
	
	obj = (*env)->NewObject(env, jc(env,&api,JC_LuaStackData), jm(env,&api,JM_LuaStackData_new));
	popStackData(L,env,&api,obj,0, n);
	ret = ( *env )->CallStaticBooleanMethod( env , cls_LuaState, mid_LuaState__luacall , 
			stateId , obj);
	n = pushStackData(L,env,&api,obj);
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
	JAPI api;

	memset(&api,0,sizeof(api));

	lua_getfield(L, LUA_GLOBALSINDEX, f); /* function to be called */
	sz = pushStackData(L, env, &api, reqp);	
    err = lua_pcall(L, sz, LUA_MULTRET, 0);	  /* call 'f' with 'sz' arguments and 'MULTRET' result */	
	sz = lua_gettop(L) - top;
	( *env )->ReleaseStringUTFChars( env , fun , f );
	popStackData(L,env,&api, reqp,top,sz);

	return err==0;
}

JNIEXPORT jstring JNICALL Java_ge_lua_LuaState__1eval
  (JNIEnv * env, jobject jThis, jlong data, jstring content)
{
	lua_State * L = getStateFromJObj( env , data );	
	int top = lua_gettop(L);
	const char* str = ( *env )->GetStringUTFChars( env, content, NULL );
	int err;
	
	err = luaL_dostring(L, str);
	( *env )->ReleaseStringUTFChars( env , content , str );
	if(err!=0) {
		const char* v = lua_tostring(L,-1);
		jstring jstr = ( *env )->NewStringUTF(env, v);
		lua_pop(L,1);
		return jstr;
	}
	return NULL;
}

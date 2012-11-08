#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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
#define LUAJAVA_STATEOBJ		"__JAVA_StateObj"

static lua_State * getStateFromJObj( JNIEnv * env , jlong cptr );

static void pushJNIEnv( JNIEnv * env , lua_State * L );

static JNIEnv * getEnvFromState( lua_State * L );

#define JC_LuaProxy	0
#define JCLASS_MAX	1

#define JM_LuaProxy_new			0
#define JM_LuaProxy_getSize		1
#define JM_LuaProxy_getType		2
#define JM_LuaProxy_getAt		3
#define JM_LuaProxy_asInt		4
#define JM_LuaProxy_asNumber	5
#define JM_LuaProxy_asString	6
#define JM_LuaProxy_asBoolean	7
#define JM_LuaProxy_reset		8
#define JM_LuaProxy_pushValue	9
#define JM_LuaProxy_fromInt		10
#define JM_LuaProxy_fromNumber	11
#define JM_LuaProxy_fromBoolean	12
#define JM_LuaProxy_nextKey		13
#define JM_LuaProxy_getType2	14
#define JM_LuaProxy_getAt2		15
#define JMETHOD_MAX	16

typedef struct {
	jclass c[JCLASS_MAX];
	jmethodID m[JMETHOD_MAX];
} JAPI;

static jclass jc(JNIEnv* env,JAPI* api,int id) {
	if(!api->c[id]) {
		jclass c = NULL;
		switch(id) {
		case JC_LuaProxy:
			c = ( *env )->FindClass( env , "ge/lua/LuaProxy" );
			break;
		}
		api->c[id] = c;
	}
	return api->c[id];
}

static jmethodID jm(JNIEnv* env, JAPI* api,int id) {
	if(!api->m[id]) {
		jmethodID m = NULL;
		switch(id) {
		case JM_LuaProxy_new:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "create", "(Z)Ljava/lang/Object;" );
			break;
		case JM_LuaProxy_getSize:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "getSize" , "()I" );	
			break;
		case JM_LuaProxy_getType:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "getType" , "(I)I" );	
			break;
		case JM_LuaProxy_getAt:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "getAt" , "(I)Ljava/lang/Object;" );	
			break;
		case JM_LuaProxy_asInt:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "asInt" , "(Ljava/lang/Object;)I" );	
			break;
		case JM_LuaProxy_asNumber:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "asNumber" , "(Ljava/lang/Object;)D" );	
			break;
		case JM_LuaProxy_asString:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "asString" , "(Ljava/lang/Object;)Ljava/lang/String;" );	
			break;
		case JM_LuaProxy_asBoolean:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "asBoolean" , "(Ljava/lang/Object;)Z" );
			break;
		case JM_LuaProxy_reset:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "reset" , "()V" );
			break;
		case JM_LuaProxy_pushValue:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "pushValue" , "(Ljava/lang/Object;)V" );
			break;
		case JM_LuaProxy_fromInt:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "fromInt" , "(I)Ljava/lang/Object;" );	
			break;
		case JM_LuaProxy_fromNumber:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "fromNumber" , "(D)Ljava/lang/Object;" );	
			break;
		case JM_LuaProxy_fromBoolean:
			m = ( *env )->GetStaticMethodID( env , jc(env,api,JC_LuaProxy), "fromBoolean" , "(Z)Ljava/lang/Object;" );
			break;
		case JM_LuaProxy_nextKey:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "nextKey" , "()Ljava/lang/String;" );
			break;
		case JM_LuaProxy_getType2:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "getType" , "(Ljava/lang/String;)I" );
			break;
		case JM_LuaProxy_getAt2:
			m = ( *env )->GetMethodID( env , jc(env,api,JC_LuaProxy), "getAt" , "(Ljava/lang/String;)Ljava/lang/Object;" );
			break;
		}
		api->m[id] = m;
	}
	return api->m[id];
}

jobject getLuaStateObj(lua_State* L) {
	jobject r;

	lua_pushstring( L , LUAJAVA_STATEOBJ );
	lua_rawget( L , LUA_REGISTRYINDEX );

	if( !lua_islightuserdata(L, -1))	
	{
		lua_pushstring( L , "Impossible to identify luaStateObject." );
		lua_error( L );
	}
	r = (jobject) lua_touserdata(L , -1);
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

static int isLuaArray(lua_State *l, int idx)
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

static void popLuaValue(lua_State* L,JNIEnv* env, JAPI* api, jobject data,int idx) {
	jobject v = NULL;
	int type = lua_type(L,idx);				
	switch(type) {
	case LUA_TBOOLEAN: {
		v = ( *env )->CallStaticObjectMethod(env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_fromBoolean),lua_toboolean(L,idx));
		break;
	}
	case LUA_TNUMBER: {
		lua_Integer v1 = lua_tointeger(L,idx);
		lua_Number v2 = lua_tonumber(L,idx);
		if(v1==v2) {
			v = ( *env )->CallStaticObjectMethod(env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_fromInt), (int) v1);
		} else {
			v = ( *env )->CallStaticObjectMethod(env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_fromNumber),v2);
		}				
		break;
	}
	case LUA_TSTRING: {
		const char* v = lua_tostring(L, idx);
		jstring jstr = ( *env )->NewStringUTF(env,v);
		( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaProxy_pushValue), (jobject) jstr);
		( *env )->DeleteLocalRef(env, jstr);
		return;
	}		
	case LUA_TTABLE: {
		int len;

		if(idx<0) {
			idx = lua_gettop(L)+idx+1;
		}
		len = isLuaArray(L, idx);
        if (len > 0) {
			int i;			
			v = (*env)->CallStaticObjectMethod(env, jc(env,api,JC_LuaProxy), jm(env,api,JM_LuaProxy_new), 0);
			for (i = 1; i <= len; i++) {		
				lua_rawgeti(L, idx, i);
				popLuaValue(L, env, api, v, -1);
				lua_pop(L, 1);
			}
		} else {
			v = (*env)->CallStaticObjectMethod(env, jc(env,api,JC_LuaProxy), jm(env,api,JM_LuaProxy_new), 1);			
			lua_pushnil(L);
			while (lua_next(L, idx) != 0) {
				popLuaValue(L, env, api, v, -2);
				popLuaValue(L, env, api, v, -1);
				lua_pop(L, 1);
			}
		}
		break;
	}
	case LUA_TNIL:
	default:		
		break;
	}
	( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaProxy_pushValue),v);
}

static void popStackData(lua_State* L,JNIEnv* env, JAPI* api, jobject data,int top,int nresults) {
	int i;
	( *env )->CallVoidMethod(env,data,jm(env,api,JM_LuaProxy_reset));
	for(i=1;i<=nresults;i++) {		
		popLuaValue(L,env,api,data,top+i);		
	}
	if(nresults>0) {
		lua_pop(L,nresults);
	}
}

static void pushJavaValue(lua_State* L,JNIEnv* env, JAPI* api, int type, jobject data) {
	switch(type) {
		case -2: {
			lua_Integer v = ( *env )->CallStaticIntMethod(env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_asInt), data);
			lua_pushinteger(L, v);
			break;
		}			
		case LUA_TBOOLEAN: {
			jboolean v = ( *env )->CallStaticBooleanMethod(env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_asBoolean),data);
			lua_pushboolean(L,v);
			break;
		}
		case LUA_TNUMBER: {
			jdouble v = ( *env )->CallStaticDoubleMethod(env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_asNumber), data);
			lua_pushnumber(L,v);
			break;
		}
		case LUA_TSTRING: {
				jstring ret;
				ret = (jstring) ( *env )->CallStaticObjectMethod( env,jc(env,api,JC_LuaProxy),jm(env,api,JM_LuaProxy_asString),data);
				if(ret!=NULL) {
					const char* c = ( *env )->GetStringUTFChars( env, ret, NULL );
					lua_pushstring(L, c);
					( *env )->ReleaseStringUTFChars( env , ret, c );
				} else {
					lua_pushnil(L);
				}
				break;
			}
		case -3: {
				int sz = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaProxy_getSize));
				int i;	 
				lua_newtable(L);
				for(i=0;i<sz;i++) {
					int type2 = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaProxy_getType),i);
					jobject v2 = ( *env )->CallObjectMethod(env,data,jm(env,api,JM_LuaProxy_getAt),i);
					lua_pushnumber(L, i+1);
					pushJavaValue(L,env,api,type2,v2);
					lua_settable(L,-3);
				}	
				break;
			}
		case LUA_TTABLE: {
				int type2;
				jobject v2;
				jstring key;

				lua_newtable(L);
				key = (jstring) ( *env )->CallObjectMethod( env,data,jm(env,api,JM_LuaProxy_nextKey));
				while(key!=NULL) {
					const char* c = ( *env )->GetStringUTFChars( env, key, NULL );
					lua_pushstring(L, c);
					( *env )->ReleaseStringUTFChars( env , key, c );

					type2 = ( *env )->CallIntMethod( env,data,jm(env,api,JM_LuaProxy_getType2), key);
					v2 = ( *env )->CallObjectMethod( env,data,jm(env,api,JM_LuaProxy_getAt2), key);
					pushJavaValue(L, env, api, type2, v2);
					lua_settable(L,-3);

					key = (jstring) ( *env )->CallObjectMethod( env,data,jm(env,api,JM_LuaProxy_nextKey));
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

static int pushStackData(lua_State* L,JNIEnv* env, JAPI* api, jobject data) {
	int sz = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaProxy_getSize));
	int i;	 
	for(i=0;i<sz;i++) {
		int type = ( *env )->CallIntMethod(env,data,jm(env,api,JM_LuaProxy_getType),i);
		jobject v = ( *env )->CallObjectMethod(env,data,jm(env,api,JM_LuaProxy_getAt),i);
		pushJavaValue(L,env,api,type,v);
	}	
	return sz;
}

/*********************** LUA API FUNCTIONS ******************************/
static int lua_javacall(lua_State *L) {		
	jobject jobj = getLuaStateObj(L);	
	JNIEnv * env = getEnvFromState( L );	
	int n = lua_gettop(L);  /* number of arguments */	
	jobject obj;
	jboolean ret;
	jclass    cls_LuaState = ( *env )->FindClass( env , "ge/lua/LuaState" );	
	jmethodID mid_LuaState__luacall	= ( *env )->GetMethodID( env , cls_LuaState, "_luacallback" , "(Lge/lua/LuaArray;)Z" );
	JAPI api;

	memset(&api,0,sizeof(api));
	
	obj = (*env)->CallStaticObjectMethod(env, jc(env,&api,JC_LuaProxy), jm(env,&api,JM_LuaProxy_new), 0);
	popStackData(L,env,&api,obj,0, n);
	ret = ( *env )->CallBooleanMethod( env , jobj, mid_LuaState__luacall , 
			obj);
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
  (JNIEnv * env , jobject jobj  /*, jint*/)
{
	jobject gobj;
	lua_State * L = NULL;

	gobj = ( *env )->NewGlobalRef( env, jobj );

	L = luaL_newstate();

	luaL_openlibs(L);  /* open libraries */
	luamodule_cjson(L);
	luamodule_cjson_safe(L);
	luamodule_md5(L);
	luamodule_des56(L);
	luaopen_java_ext(L); 	
	
	lua_pushstring( L , LUAJAVA_STATEOBJ );
	lua_pushlightuserdata(L, (void*) gobj);
	// lua_pushnumber( L , (lua_Number)stateId );
	lua_settable( L , LUA_REGISTRYINDEX );

	pushJNIEnv(env, L);   

	return (jlong) L;
}

JNIEXPORT void JNICALL Java_ge_lua_LuaState__1close
  (JNIEnv * env , jobject jobj , jlong cptr)
{
	lua_State * L = getStateFromJObj( env , cptr );
	jobject gobj = getLuaStateObj(L);
	( *env )->DeleteGlobalRef(env, gobj);
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

	pushJNIEnv(env, L);
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

	pushJNIEnv(env, L);
	
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

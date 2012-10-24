#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "thr_queue.h"
#include "lua_bgthread.h"

struct reqrep {
	int id;
	int err;
	char* data;
};

#define MAX_QUEUE	20

static pthread_t bgpid;
static thr_queue_t queueReq;
static thr_queue_t queueRep;

static lua_State* bgState;
static log_luaerror logger;

const char* EMPTY = "";

void *bgthread_main(void* script)
{
	struct reqrep* work = 0;
	int nRet = 0;
	const char * buf;

	thr_queue_push(&queueRep, (void *) 0);
	
    while (1)
    {
		thr_queue_pop(&queueReq, (void **)&work);
        if ( work == NULL ) break;

		if(work->id==0) {
			nRet =  luaL_dostring(bgState, (char*) work->data);
			lua_gc(bgState, LUA_GCCOLLECT, 0);			
		} else {
			lua_getfield(bgState, LUA_GLOBALSINDEX, "bg_dispatcher");	/* function to be called */
			lua_pushinteger(bgState, work->id);							/* 1st argument */
			lua_pushstring(bgState, work->data);						/* 2st argument */		
			nRet = lua_pcall(bgState, 2, 1, 0);
		}		
		if (nRet != 0) {
			work->err = nRet;
			buf = logger(bgState);
		} else {
			work->err = 0;
			buf = lua_tostring(bgState,-1);			
		}
		if(work->data!=NULL) {
			free(work->data);
			work->data = NULL;
		}
		if(buf==NULL)buf=EMPTY;
		work->data = malloc(strlen(buf)+1);
		strcpy(work->data,buf);
		// free(work);
		lua_pop(bgState,1);

		thr_queue_push(&queueRep, work);
    }

    return NULL;
}

int bgthread_start(lua_State* state,log_luaerror log) 
{
	int r = -1;

	bgState = state;
	logger = log;

	thr_queue_create(&queueReq, MAX_QUEUE);
	thr_queue_create(&queueRep, MAX_QUEUE);

	pthread_create(&bgpid, NULL, bgthread_main,NULL);
	
	thr_queue_pop(&queueRep, (void **)&r);
	return r;
}

void bgthread_stop() {
	thr_queue_push(&queueReq, NULL);
	pthread_join(bgpid, NULL);

	thr_queue_destroy(&queueReq);
	thr_queue_destroy(&queueRep);
}

static void bgthread_docall(int id, const char* s) {
	struct reqrep* work = malloc(sizeof(struct reqrep));
	work->id = id;
	work->data = malloc(strlen(s)+1);
	strcpy(work->data,s);
	work->err = 0;	
	thr_queue_push(&queueReq, work);
}


void bgthread_initcall(const char* content) {
	struct reqrep* work = NULL;
	bgthread_docall(0,content);

	thr_queue_pop(&queueRep,(void **)&work);

	if(work!=NULL) {
		if(work->data!=NULL) {
			 free(work->data);
		}
		free(work);
	}
}

static int bgmain_call(lua_State *L)
{
	if (lua_isnumber(L, 1) && lua_isstring(L,2))
	{
		int id = luaL_checkinteger(L, 1);
		const char* s=luaL_checkstring(L, 2);
		bgthread_docall(id,s);		
		return 0;
	} else
	{
		luaL_argerror(L, 1, "string parameter expected");
	}
	return 0;
}

static int bgthread_peek(lua_State *L)
{
	struct reqrep* work = NULL;
	if(thr_queue_peek(&queueRep,(void **)&work)==0) {
		lua_pushinteger(L,0); // has return
		lua_pushinteger(L,0); // id		
		lua_pushinteger(L,0); // err		
		lua_pushstring(L,""); // data		
	} else {		
		lua_pushinteger(L,1); // has return
		lua_pushinteger(L,work->id); // id		
		lua_pushinteger(L,work->err); // id		
		lua_pushstring(L,work->data); // data
		if(work->data!=NULL) {
			free(work->data);
			work->data = NULL;
		}
		free(work);		
	}
	return 4;	
}

int luaopen_bgthread_main(lua_State *L)
{
   luaL_Reg reg[] = {
        { "call", bgmain_call },
		{ "peek", bgthread_peek },
        { NULL, NULL }
    };
	luaL_openlib (L, "bgthread__api", reg, 0);
    return 1;
}

void luamodule_bgthread_main(lua_State *L)
{
	luaopen_bgthread_main(L);
}

/* vi:ai et sw=4 ts=4:
 */

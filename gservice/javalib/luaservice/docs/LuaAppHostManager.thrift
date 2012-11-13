/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  byte        Signed byte
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */
namespace php ge.lua.service.thrift
namespace java ge.lua.service.thrift

struct TLuaAppInfo {
  1: string id,
  2: string type,
  3: string createTime,
}

struct TLuaAppCallResult {
  1: bool done,
  2: string result,
}

service TLuaAppHostManager {

	bool createApp(1:string appId, 2:string appType),

	bool restartApp(1:string appId),

	bool closeApp(1:string appId, 2:bool destroy),
	
	TLuaAppInfo getApp(1:string appId),

	list<TLuaAppInfo> listApp(),
	
	TLuaAppCallResult appCall(1:string appId, 2:string name, 3:string params),
	
	TLuaAppCallResult appAICall(1:string appId, 2:string name, 3:string params, 4:i32 timeout),
	
	bool appEval(1:string appId, 2:string content),
}
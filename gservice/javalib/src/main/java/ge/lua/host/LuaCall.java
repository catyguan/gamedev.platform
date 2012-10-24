package ge.lua.host;

import ge.lua.LuaStackData;

public interface LuaCall {

	/**
	 * 
	 * @param app
	 * @param callId
	 * @param data
	 * @return true/同步响应,false/异步响应
	 */
	public boolean call(LuaApp app, int callId, LuaStackData data);
}

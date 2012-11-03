package ge.lua;

public interface LuaStateCallback {

	/**
	 * 
	 * @param state
	 * @param data
	 * @return false 表示错误
	 */
	public boolean callback(LuaState state, LuaStackData data);
}

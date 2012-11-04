/*
 * $Id: LuaState.java,v 1.11 2007-09-17 19:28:40 thiago Exp $
 * Copyright (C) 2003-2007 Kepler Project.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

package ge.lua;

/**
 * 对应Lua_State对象
 * 
 * @author guanzhong
 * 
 */
public class LuaState {

	private final static String LUAJAVA_LIB = "luajni";

	/**
	 * Opens the library containing the luajava API
	 */
	static {
		System.loadLibrary(LUAJAVA_LIB);
	}

	/********************* Lua Native Interface *************************/
	private native long _open(int stateId);

	private native void _threadInit(long ptr);

	private native void _close(long ptr);

	private native boolean _addpath(long luaState, String path);

	private native boolean _setvar(long luaState, String key, String value);

	private native boolean _pcall(long luaState, String funName,
			LuaArray data);

	private native String _eval(long luaState, String content);

	/********************* end Lua Native Interface *********************/

	private long luaState;

	private int stateId;

	/**
	 * Constructor to instance a new LuaState and initialize it with LuaJava's
	 * functions
	 * 
	 * @param stateId
	 */
	protected LuaState(int stateId) {
		luaState = _open(stateId);
		this.stateId = stateId;
	}

	private LuaStateCallback callback;

	public void setCallback(LuaStateCallback cb) {
		callback = cb;
	}

	public LuaStateCallback getCallback() {
		return callback;
	}

	public static boolean _luacallback(int stateId, LuaArray data) {
		try {
			LuaState st = LuaStateManager.getState(stateId);
			if (st == null) {
				return data.error("luaState[" + stateId + "] invalid");
			}
			if (st.callback == null) {
				return data.error("callback is null");
			}
			return st.callback.callback(st, data);
		} catch (Throwable t) {
			return data.error(t.getMessage());
		}
	}

	private Object context;

	public Object getContext() {
		return context;
	}

	public void setContext(Object context) {
		this.context = context;
	}

	public boolean addpath(String path) {
		if (this.luaState == 0)
			throw new NullPointerException("luaState");
		return _addpath(this.luaState, path);
	}

	public boolean setvar(String key, String value) {
		if (this.luaState == 0)
			throw new NullPointerException("luaState");
		return _setvar(this.luaState, key, value);
	}

	public boolean pcall(String funName, LuaArray data) {
		return pcall(funName, data, false);
	}

	public boolean pcall(String funName, LuaArray data, boolean throwError) {
		if (this.luaState == 0)
			throw new NullPointerException("luaState");
		boolean r = _pcall(this.luaState, funName, data);
		if (!r && throwError) {
			String msg = data.getString(0);
			throw new LuaStateException(msg);
		}
		return r;
	}

	public String eval(String content, boolean throwError) {
		if (this.luaState == 0)
			throw new NullPointerException("luaState");
		String msg = _eval(this.luaState, content);
		if (msg != null && throwError) {
			throw new LuaStateException(msg);
		}
		return msg;
	}

	/**
	 * Closes state and removes the object from the LuaStateFactory
	 */
	public void close() {
		if (this.luaState != 0) {
			LuaStateManager.removeState(stateId);
			_close(luaState);
			this.luaState = 0;
		}
	}

	public void threadInit() {
		if (this.luaState != 0) {
			_threadInit(luaState);
		}
	}

	@Override
	protected void finalize() throws Throwable {
		close();
		super.finalize();
	}

	@Override
	public String toString() {
		return "LuaState[" + stateId + ",0x" + Long.toHexString(luaState) + "]";
	}

	/**
	 * Returns <code>true</code> if state is closed.
	 */
	public boolean isOpen() {
		return luaState != 0;
	}

}

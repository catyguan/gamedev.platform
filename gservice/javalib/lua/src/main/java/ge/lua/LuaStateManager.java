/*
 * $Id: LuaStateFactory.java,v 1.4 2006-12-22 14:06:40 thiago Exp $
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

import java.util.ArrayList;

/**
 * Lua的状态机管理对象
 * 
 * @author guanzhong
 * 
 */
public final class LuaStateManager {

	/**
	 * Array with all luaState's instances
	 */
	private static final ArrayList<LuaState> states = new ArrayList<LuaState>();

	/**
	 * Method that creates a new instance of LuaState
	 * 
	 * @return LuaState
	 */
	public synchronized static LuaState newState() {
		int i = getNextStateIndex();
		LuaState L = new LuaState(i);
		states.add(i, L);
		return L;
	}

	/**
	 * Returns a existing instance of LuaState
	 * 
	 * @param index
	 * @return LuaState
	 */
	public synchronized static LuaState getState(int index) {
		return (LuaState) states.get(index);
	}

	/**
	 * removes the luaState from the states list
	 * 
	 * @param idx
	 */
	public synchronized static void removeState(int idx) {
		if (idx < states.size()) {
			states.set(idx, null);
		}
	}

	/**
	 * Get next available index
	 * 
	 * @return int
	 */
	private synchronized static int getNextStateIndex() {
		int i;
		for (i = 0; i < states.size() && states.get(i) != null; i++)
			;
		return i;
	}
}
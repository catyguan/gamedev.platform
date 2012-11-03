package ge.lua.host.impl;

import ge.lua.LuaStackData;
import ge.lua.host.LuaApp;
import bma.common.langutil.ai.stack.AIStackROOT;
import bma.common.langutil.core.ValueUtil;

public class AIStackLuaResponse extends AIStackROOT<LuaStackData> {

	protected LuaApp app;
	protected int callId;

	public AIStackLuaResponse(LuaApp app, int callId) {
		super();
		this.app = app;
		this.callId = callId;
	}

	public LuaApp getApp() {
		return app;
	}

	public int getCallId() {
		return callId;
	}

	@Override
	public boolean end(LuaStackData result, Throwable t) {
		if (t != null) {
			String msg = t.getMessage();
			if (ValueUtil.empty(msg))
				msg = "error-" + t.getClass().getName();
			app.luaCallResponse(callId, null, msg);
		} else {
			if (result == null)
				result = new LuaStackData();
			app.luaCallResponse(callId, result, null);
		}
		return true;
	}
}

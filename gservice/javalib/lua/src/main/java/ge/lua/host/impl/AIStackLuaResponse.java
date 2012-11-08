package ge.lua.host.impl;

import ge.lua.LuaArray;
import ge.lua.host.LuaApp;
import bma.common.langutil.ai.stack.AIStackROOT;
import bma.common.langutil.core.ValueUtil;

public class AIStackLuaResponse extends AIStackROOT<LuaArray> {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(AIStackLuaResponse.class);

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
	public boolean end(LuaArray result, Throwable t) {
		if (app != null) {
			if (t != null) {
				String msg = t.getMessage();
				if (ValueUtil.empty(msg))
					msg = "error-" + t.getClass().getName();
				app.luaCallResponse(callId, msg, null);
			} else {
				if (result == null)
					result = new LuaArray();
				app.luaCallResponse(callId, null, result);
			}
			return true;
		} else {
			if (log.isDebugEnabled()) {
				log.debug("app is null, skip [{},{}]", result, t);
			}
			return true;
		}
	}
}

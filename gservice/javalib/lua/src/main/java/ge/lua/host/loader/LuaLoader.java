package ge.lua.host.loader;

import ge.lua.LuaArray;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaAppBuilder;
import ge.lua.host.LuaCallWithName;
import bma.common.langutil.core.ValueUtil;

public abstract class LuaLoader implements LuaCallWithName, LuaAppBuilder {

	public static final String METHOD_NAME = "loader";

	@Override
	public String getName() {
		return METHOD_NAME;
	}

	public static class Chunk {
		public String name;
		public String content;
	}

	@Override
	public boolean call(LuaApp app, int callId, LuaArray data) {
		String moduleName = data.getString(0);
		data.reset();
		Chunk chunk = loadChunk(moduleName);
		if (chunk != null && chunk.content != null) {
			data.addString(chunk.content);
			if (ValueUtil.notEmpty(chunk.name)) {
				data.addString(chunk.name);
			}
		}
		return true;
	}

	public abstract Chunk loadChunk(String moduleName);

	@Override
	public void buildApp(LuaApp app, boolean afterBootstap) {
		if (!afterBootstap) {
			initApp(app);
		}
	}

	public void initApp(LuaApp app) {
		if (!app.getHost().hasCall(METHOD_NAME)) {
			app.getHost().addCall(METHOD_NAME, this);
		}
		StringBuilder sb = new StringBuilder();
		sb.append("local f = function(modulename)\n");
		sb.append("local sync,fcontent, fname = java.call(\"hostCall\", 1");
		sb.append(",\"").append(METHOD_NAME).append("\"");
		sb.append(",modulename");
		sb.append(")\n");
		if (true) {
			sb.append("if fcontent then\n");
			sb.append("if not fname then fname = modulename end\n");
			sb.append("return assert(loadstring(assert(fcontent), fname))\n");
			sb.append("end\n");
		}
		sb.append("end\n");
		sb.append("table.insert(package.loaders, 2, f)\n");
		app.eval(sb.toString());
	}
}

package ge.lua.hostservice.gcommand;

import ge.lua.LuaArray;
import ge.lua.appcommand.AppCommandClient;
import ge.lua.appcommand.Dispatcher;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaCallWithName;
import ge.lua.host.impl.AIStackLuaResponse;

import java.util.List;

import bma.common.json.JsonUtil;
import bma.common.langutil.ai.AIUtil;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.ValueUtil;

public class ServiceLuaCall implements LuaCallWithName {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ServiceLuaCall.class);

	public static final String METHOD_NAME = "gcommand.execute";

	@Override
	public String getName() {
		return METHOD_NAME;
	}

	private AppCommandClient client;

	public ServiceLuaCall() {
		super();
	}

	public AppCommandClient getClient() {
		return client;
	}

	public void setClient(AppCommandClient client) {
		this.client = client;
	}

	@Override
	public boolean call(final LuaApp app, final int callId, final LuaArray data)
			throws Exception {

		String accessId = data.getString(0);
		String sceneName = data.getString(1);
		String commandName = data.getString(2);
		LuaArray params = data.getArray(3);
		int tm = data.getInt(4);
		data.reset();

		if (ValueUtil.empty(accessId)) {
			throw new IllegalArgumentException("accessId invalid");
		}
		if (ValueUtil.empty(sceneName)) {
			throw new IllegalArgumentException("secne name invalid");
		}
		if (ValueUtil.empty(commandName)) {
			throw new IllegalArgumentException("command name invalid");
		}
		AIStack<LuaArray> root = new AIStackLuaResponse(app, callId);
		AIStackStep<String, LuaArray> stack = new AIStackStep<String, LuaArray>(
				root) {

			@Override
			protected boolean next(String result) {
				try {
					List a = JsonUtil.getDefaultMapper().readValue(result,
							List.class);
					LuaArray r = new LuaArray();
					r.bind(a);
					return successForward(r);
				} catch (Exception e) {
					return AIUtil.safeFailure(delegate(), e);
				}
			}

		};

		Dispatcher disp = null;
		if (app != null) {
			disp = new Dispatcher() {

				@Override
				public boolean find(AIStack<String> stack, String name) {
					LuaArray ps = new LuaArray();
					ps.addString(name);
					AIStackStep<LuaArray, String> step = new AIStackStep<LuaArray, String>(
							stack) {

						@Override
						protected boolean next(LuaArray result) {
							return successForward(result.getString(0));
						}
					};
					return app.runCall(step, "gcommandDispatcher", ps);
				}
			};
		}
		return client.execute(stack, disp, accessId, sceneName, commandName,
				params, tm);
	}

}

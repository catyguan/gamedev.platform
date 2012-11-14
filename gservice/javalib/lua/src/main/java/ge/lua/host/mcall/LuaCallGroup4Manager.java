package ge.lua.host.mcall;

import ge.lua.LuaArray;
import ge.lua.LuaStateException;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaAppHost;
import ge.lua.host.LuaCall;
import ge.lua.host.LuaCallGroup;
import ge.lua.host.impl.AIStackLuaResponse;
import bma.common.langutil.ai.stack.AIStackStep;

public class LuaCallGroup4Manager implements LuaCallGroup {

	@Override
	public void buildLuaCall(LuaAppHost host) {
		host.addCall("host.createApp", new LuaCall() {

			@Override
			public boolean call(LuaApp app, int callId, LuaArray data)
					throws Exception {
				String appId = data.getString(0);
				String appType = data.getString(1);

				data.reset();
				LuaApp napp = app.getHost().createApp(appId, appType);
				data.addBoolean(napp != null);
				return true;
			}
		});

		host.addCall("host.restartApp", new LuaCall() {

			@Override
			public boolean call(LuaApp app, int callId, LuaArray data)
					throws Exception {
				final String appId = data.getString(0);
				data.reset();
				final LuaApp tapp = app.getHost().queryApp(appId);
				if (tapp == null) {
					data.addBoolean(false);
					return true;
				}

				final String type = app.getType();

				AIStackLuaResponse stack = new AIStackLuaResponse(app, callId);
				AIStackStep<Boolean, LuaArray> step = new AIStackStep<Boolean, LuaArray>(
						stack) {

					@Override
					protected boolean next(Boolean result) {
						try {
							LuaApp napp = tapp.getHost().createApp(appId, type);
							LuaArray r = new LuaArray();
							r.addBoolean(napp != null);
							return successForward(r);
						} catch (Exception e) {
							return failure(e);
						}
					}
				};
				return app.getHost().closeApp(step, appId, false);
			}
		});

		host.addCall("host.closeApp", new LuaCall() {

			@Override
			public boolean call(LuaApp app, int callId, LuaArray data)
					throws Exception {
				final String appId = data.getString(0);
				boolean destroy = data.getBoolean(1);

				data.reset();
				AIStackLuaResponse stack = new AIStackLuaResponse(app, callId);
				AIStackStep<Boolean, LuaArray> step = new AIStackStep<Boolean, LuaArray>(
						stack) {

					@Override
					protected boolean next(Boolean result) {
						try {
							LuaArray r = new LuaArray();
							r.addBoolean(true);
							return successForward(r);
						} catch (Exception e) {
							return failure(e);
						}
					}
				};
				return app.getHost().closeApp(step, appId, destroy);
			}
		});

		host.addCall("host.appCall", new LuaCall() {

			@Override
			public boolean call(LuaApp app, int callId, LuaArray data)
					throws Exception {
				String appId = data.getString(0);
				LuaApp tapp = app.getHost().queryApp(appId);
				if (tapp == null) {
					throw new LuaStateException("app[" + appId + "] not exists");
				}
				
				String name = data.getString(1);
				data.pop(0);
				data.pop(0);
				LuaArray params = data.copy();
				data.reset();

				AIStackLuaResponse stack = new AIStackLuaResponse(app, callId);
				return tapp.runCall(stack, name, params);
			}
		});
	}
}

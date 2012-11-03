package ge.lua.service.impl;

import ge.lua.host.LuaApp;
import ge.lua.host.LuaAppHost;
import ge.lua.service.thrift.TLuaAppHostManager4AI.Iface;
import ge.lua.service.thrift.TLuaAppInfo;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.apache.thrift.TException;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.DateTimeUtil;

public class TLuaAppHostManager4AIThrift implements Iface {

	private LuaAppHost host;

	public LuaAppHost getHost() {
		return host;
	}

	public void setHost(LuaAppHost host) {
		this.host = host;
	}

	@Override
	public boolean createApp(AIStack<Boolean> stack, String appId,
			String appType) throws TException {
		LuaApp app = host.createApp(appId, appType);
		return stack.success(app != null);
	}

	@Override
	public boolean restartApp(AIStack<Boolean> stack, final String appId)
			throws TException {
		LuaApp app = host.queryApp(appId);
		if (app == null) {
			return stack.success(false);
		}
		final String type = app.getType();
		AIStackStep<Boolean, Boolean> step = new AIStackStep<Boolean, Boolean>(
				stack) {

			@Override
			protected boolean next(Boolean result) {
				try {
					return createApp(delegate(), appId, type);
				} catch (Exception e) {
					return failure(e);
				}
			}
		};
		return host.closeApp(step, appId, false);
	}

	@Override
	public boolean closeApp(AIStack<Boolean> stack, String appId)
			throws TException {
		return host.closeApp(stack, appId, true);
	}

	public static TLuaAppInfo create(LuaApp app) {
		TLuaAppInfo r = new TLuaAppInfo();
		if (app != null) {
			r.setId(app.getId());
			r.setType(app.getType());
			r.setCreateTime(DateTimeUtil.formatDateTime(new Date(app
					.getCreateTime())));
		}
		return r;
	}

	@Override
	public boolean getApp(AIStack<TLuaAppInfo> stack, String appId)
			throws TException {
		LuaApp app = host.queryApp(appId);
		return stack.success(create(app));
	}

	@Override
	public boolean listApp(AIStack<List<TLuaAppInfo>> stack) throws TException {
		List<LuaApp> list = host.listApp();
		List<TLuaAppInfo> r = new ArrayList<TLuaAppInfo>(list.size());
		for (LuaApp app : list) {
			r.add(create(app));
		}
		return stack.success(r);
	}
}

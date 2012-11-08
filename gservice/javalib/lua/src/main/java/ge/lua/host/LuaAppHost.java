package ge.lua.host;

import ge.lua.LuaArray;
import ge.lua.LuaState;
import ge.lua.LuaStateCallback;
import ge.lua.host.LuaApp.Command;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.codehaus.jackson.map.ObjectMapper;

import bma.common.json.JsonUtil;
import bma.common.langutil.ai.executor.AIExecutor;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackNone;
import bma.common.langutil.concurrent.ProcessTimerTask;
import bma.common.langutil.concurrent.TimerManager;
import bma.common.langutil.core.ObjectUtil;
import bma.common.langutil.core.StringUtil;
import bma.common.langutil.core.ValueUtil;

public class LuaAppHost {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(LuaAppHost.class);

	protected AIExecutor executor;
	protected TimerManager timer;
	protected ObjectMapper mapper;
	protected int mapSize = 100;
	protected LuaAppFactory factory;
	protected Map<String, LuaCall> calls;
	protected List<LuaAppBuilder> appBuilders;

	// runtime
	protected Map<String, LuaApp> apps;
	protected LinkedList<LuaApp> closingApps;

	public List<LuaAppBuilder> getAppBuilders() {
		return appBuilders;
	}

	public void setAppBuilders(List<LuaAppBuilder> appBuilders) {
		this.appBuilders = appBuilders;
	}

	public void addAppBuilder(LuaAppBuilder b) {
		if (this.appBuilders == null)
			this.appBuilders = new ArrayList<LuaAppBuilder>();
		this.appBuilders.add(b);
	}

	public void setAppBuilder(LuaAppBuilder b) {
		addAppBuilder(b);
	}

	public Map<String, LuaCall> getCalls() {
		return calls;
	}

	public void setCalls(Map<String, LuaCall> calls) {
		this.calls = calls;
	}

	public LuaCall getCall(String name) {
		return this.calls == null ? null : this.calls.get(name);
	}

	public void setNamedCalls(List<LuaCallWithName> calls) {
		for (LuaCallWithName c : calls) {
			addCall(c.getName(), c);
		}
	}

	public boolean hasCall(String name) {
		return this.calls.containsKey(name);
	}

	public void addCall(String name, LuaCall call) {
		if (this.calls == null)
			this.calls = new HashMap<String, LuaCall>();
		this.calls.put(name, call);
	}

	public LuaAppFactory getFactory() {
		return factory;
	}

	public void setFactory(LuaAppFactory factory) {
		this.factory = factory;
	}

	public ObjectMapper getMapper() {
		return mapper;
	}

	public void setMapper(ObjectMapper mapper) {
		this.mapper = mapper;
	}

	public AIExecutor getExecutor() {
		return executor;
	}

	public void setExecutor(AIExecutor executor) {
		this.executor = executor;
	}

	public Map<String, LuaApp> getApps() {
		return apps;
	}

	public void setApps(Map<String, LuaApp> apps) {
		this.apps = apps;
	}

	public int getMapSize() {
		return mapSize;
	}

	public void setMapSize(int mapSize) {
		this.mapSize = mapSize;
	}

	public TimerManager getTimer() {
		return timer;
	}

	public TimerManager sureTmer() {
		return AIExecutor.getTimerManager(timer);
	}

	public void setTimer(TimerManager timer) {
		this.timer = timer;
	}

	public void init() {
		this.apps = new ConcurrentHashMap<String, LuaApp>(this.mapSize);
		this.closingApps = new LinkedList<LuaApp>();
		if (mapper == null) {
			mapper = JsonUtil.getDefaultMapper();
		}
	}

	@Override
	protected void finalize() throws Throwable {
		close();
		super.finalize();
	}

	public void close() {
		if (apps != null) {
			List<String> keys = new ArrayList<String>(apps.keySet());
			for (String k : keys) {
				closeApp(new AIStackNone<Boolean>(), k, false);
			}
			long tm = System.currentTimeMillis();
			long lm = tm;
			while (System.currentTimeMillis() - tm < 30 * 1000) {
				int c = 0;
				synchronized (this) {
					c = closingApps.size();
					if (c == 0)
						break;
				}
				ObjectUtil.waitFor(this, 10);
				if (System.currentTimeMillis() - lm > 1000) {
					lm = System.currentTimeMillis();
					log.debug("waiting apps(" + c + ") close");
				}
			}
		}
	}

	protected void initApp(LuaApp app, boolean afterBootstap) {
		if (appBuilders != null) {
			for (LuaAppBuilder b : appBuilders) {
				b.buildApp(app, afterBootstap);
			}
		}
		if (this.calls != null) {
			for (Map.Entry<String, LuaCall> e : this.calls.entrySet()) {
				if (e.getValue() instanceof LuaAppBuilder) {
					LuaAppBuilder b = (LuaAppBuilder) e.getValue();
					b.buildApp(app, afterBootstap);
				}
			}
		}
	}

	public LuaApp createApp(String id, String type) {

		synchronized (this) {
			LuaApp app = queryApp(id);
			if (app != null) {
				if (!StringUtil.equals(type, app.getType())) {
					throw new IllegalArgumentException("[" + id + "] exists");
				}
			} else {
				app = factory.createApp(id, type);
				if (app != null) {
					if (log.isInfoEnabled()) {
						log.info("createApp({},{})", id, type);
					}

					app.setId(id);
					app.setType(type);
					app.setHost(this);

					apps.put(id, app);

					LuaState L = new LuaState();
					app.setState(L);

					L.setContext(id);
					final LuaApp fApp = app;
					L.setCallback(new LuaStateCallback() {

						@Override
						public boolean callback(LuaState state, LuaArray data) {
							return luaCall(fApp, data);
						}
					});
					L.setvar("LUA_APP_ID", id);
					L.setvar("LUA_APP_TYPE", type);

					if (app.getPathList() != null) {
						for (String path : app.getPathList()) {
							L.addpath(path);
						}
					}

					this.initApp(app, false);

					if (app.getBootstrapList() != null) {
						for (String boot : app.getBootstrapList()) {
							LuaArray data = new LuaArray();
							data.addString(boot);
							L.pcall("require", data);
							if (log.isDebugEnabled()) {
								log.debug("boot({}) => {}", boot, data);
							}
						}
					}

					this.initApp(app, true);

					if (app.getLaunchList() != null) {
						for (String pack : app.getLaunchList()) {
							final String p = pack;
							app.runCommand(new Command() {

								@Override
								public void process(LuaApp app) {
									app.require(p);
									if (log.isDebugEnabled()) {
										log.debug("launch => {}", p);
									}
								}
							});
						}
					}

				} else {
					throw new IllegalArgumentException("create [" + id + ","
							+ type + "] fail");
				}
			}
			return app;
		}
	}

	private class RUN implements Runnable {

		public LuaApp app;

		public RUN(LuaApp app) {
			super();
			this.app = app;
		}

		@Override
		public void run() {
			Command cmd;
			synchronized (app) {
				cmd = app.getQueue().poll();
				if (cmd == null) {
					app.setRun(false);
					return;
				}
			}
			try {
				cmd.process(app);
			} catch (Exception e) {
				log.error("app execute fail", e);
			}
			synchronized (app) {
				if (app.getQueue().isEmpty()) {
					if (log.isDebugEnabled()) {
						log.debug("{} stop run", app);
					}
					app.setRun(false);
					return;
				}
			}
			if (log.isDebugEnabled()) {
				log.debug("{} keep run", app);
			}
			executor.execute(this);
		}

	}

	public boolean closeApp(AIStack<Boolean> stack, String id,
			final boolean destroy) {
		LuaApp app = null;
		synchronized (this) {
			app = queryApp(id);
			if (app != null) {
				apps.remove(id);
				closingApps.add(app);
			}

		}
		if (app == null)
			return stack.success(false);

		runAppCommand(app, new Command() {

			@Override
			public void process(LuaApp app) {
				LuaState L = app.getState();
				if (L != null && L.isOpen()) {
					if (log.isInfoEnabled()) {
						log.info("{} closing", app);
					}
					if (ValueUtil.notEmpty(app.getShutdown())) {
						LuaArray data = new LuaArray();
						data.addBoolean(destroy);
						try {
							L.pcall(app.getShutdown(), data);
							if (log.isDebugEnabled()) {
								log.debug("shutdown({}) => {}",
										app.getShutdown(), data);
							}
						} catch (Exception e) {
							if (log.isWarnEnabled()) {
								log.warn("shutdown(" + app.getShutdown()
										+ ") fail", e);
							}
						}
					}
					L.close();
				}
				app.setState(null);
				closingApps.remove(app);
			}
		});
		return stack.success(true);
	}

	public void runAppCommand(LuaApp app, Command command) {
		synchronized (app) {
			app.getQueue().add(command);
			if (!app.isRun()) {
				app.setRun(true);
				executor.execute(new RUN(app));
			}
		}
	}

	public LuaApp queryApp(String id) {
		LuaApp app = this.apps.get(id);
		if (log.isDebugEnabled()) {
			log.debug("queryApp({}) => {}", id, app);
		}
		return app;
	}

	public List<LuaApp> listApp() {
		return new ArrayList<LuaApp>(this.apps.values());
	}

	public boolean luaCall(LuaApp app, LuaArray data) {
		String name = ObjectUtil.toString(data.pop(0));
		if (StringUtil.equals(name, "hostCall")) {
			String tmp = ObjectUtil.toString(data.pop(0));
			int cid = ValueUtil.intValue(tmp, 0);
			if (cid > 0) {
				String method = ObjectUtil.toString(data.pop(0));
				if (calls != null) {
					LuaCall call = calls.get(method);
					if (call != null) {
						boolean r = false;
						try {
							r = call.call(app, cid, data);
						} catch (Exception e) {
							if (log.isDebugEnabled()) {
								log.debug("call fail", e);
							}
							return data.error(e.getMessage());
						}
						if (r) {
							data.addBoolean(0, true);
						} else {
							data.reset();
							data.addBoolean(false);
						}
						return true;
					}
				}
				return data.error("invalid method '" + method + "'");
			}
			return data.error("invalid callId '" + tmp + "'");
		} else if (StringUtil.equals(name, "hostTimer")) {
			final int tid = data.getInt(0);
			if (tid > 0) {
				Number delay = data.getNumber(1);
				if (delay != null) {
					if (log.isDebugEnabled()) {
						log.debug("setTimer - {},{}", tid, delay);
					}
					final LuaApp capp = app;
					ProcessTimerTask task = TimerManager.delaySync(
							new Runnable() {

								@Override
								public void run() {
									capp.runCommand(new Command() {

										@Override
										public void process(LuaApp app) {
											LuaArray data = new LuaArray();
											data.addInt(tid);
											LuaState L = app.getState();
											boolean r = L.pcall(
													"luaTimerResponse", data,
													false);
											if (log.isDebugEnabled()) {
												log.debug("timer[{}] => {}-{}",
														new Object[] { tid, r,
																data });
											}
										}
									});

								}
							}, delay.intValue());
					sureTmer().postTimerTask(task);
					data.reset();
					return true;
				}
				return data.error("invalid timer delay");
			}
			return data.error("invalid timerId '" + tid + "'");
		}
		return data.error("invalid call '" + name + "'");
	}

	public void luaCallResponse(LuaApp app, final int callId,
			final String error, final LuaArray data) {
		runAppCommand(app, new Command() {

			@Override
			public void process(LuaApp app) {
				LuaState L = app.getState();
				if (L != null && L.isOpen()) {
					if (log.isDebugEnabled()) {
						log.debug("{} lua-java response ({},{},{})",
								new Object[] { app, callId, error, data });
					}
					LuaArray ls = data;
					try {
						if (ls == null)
							ls = new LuaArray();
						ls.addInt(0, callId);
						if (error != null) {
							ls.addString(1, error);
						} else {
							ls.addNull(1);
						}
						L.pcall("luaCallResponse", ls);
					} catch (Exception e) {
						if (log.isWarnEnabled()) {
							log.warn("luaCallResponse fail", e);
						}
					}
				}
			}
		});
	}

}

package ge.lua.host.ai;

import ge.lua.LuaStackData;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaApp.Command;
import ge.lua.host.LuaCall;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentSkipListMap;
import java.util.concurrent.TimeoutException;

import bma.common.langutil.ai.AIUtil;
import bma.common.langutil.ai.executor.AIExecutor;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackOne;
import bma.common.langutil.concurrent.ProcessTimerTask;
import bma.common.langutil.concurrent.TimerManager;
import bma.common.langutil.core.RoundRobinInteger;

public class LuaAICall implements LuaCall {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(LuaAICall.class);

	public static final String METHOD_NAME = "airesponse";

	protected Map<Integer, AIStack<LuaStackData>> stacks = new ConcurrentSkipListMap<Integer, AIStack<LuaStackData>>();
	protected RoundRobinInteger callId = new RoundRobinInteger(10000000);

	public int aicall(final AIStack<LuaStackData> stack, LuaApp app,
			final String funName, final LuaStackData req, long timeout,
			TimerManager tm) {
		final AIStack<LuaStackData> callStack = new AIStackOne<LuaStackData>(
				stack);
		final int callId = aicall(callStack, app, funName, req);
		ProcessTimerTask task = TimerManager.delay(new Runnable() {

			@Override
			public void run() {
				if (discard(callId)) {
					callStack.failure(new TimeoutException("LuaAICall-"
							+ callId));
				}
			}
		}, timeout);
		AIExecutor.getTimerManager(tm).postTimerTask(task);
		return callId;
	}

	public int aicall(final AIStack<LuaStackData> stack, LuaApp app,
			final String funName, final LuaStackData req) {
		final int cid = callId.next();
		req.addInt(0, cid);
		app.runCommand(new Command() {

			@Override
			public void process(LuaApp app) {
				if (log.isDebugEnabled()) {
					log.debug("[{}] aicall({},{})", new Object[] { cid,
							funName, req });
				}
				try {
					stacks.put(cid, stack);
					app.pcall(funName, req);
					if (!req.getBoolean(0) && stacks.containsKey(cid)) {
						if (log.isDebugEnabled()) {
							log.debug(
									"call response invalid,remove stack - {}",
									req);
						}
						stacks.remove(cid);
					}
				} catch (Exception e) {
					if (log.isDebugEnabled()) {
						log.debug("call fail,remove stack - {}", e.getMessage());
					}
					stacks.remove(cid);
					AIUtil.safeFailure(stack, e);
				}
			}
		});

		return cid;
	}

	public boolean discard(int callId) {
		boolean r = stacks.remove(callId) != null;
		if (log.isDebugEnabled()) {
			log.debug("discard stack[{}] => {}", callId, r);
		}
		return r;
	}

	@Override
	public boolean call(LuaApp app, int callId, LuaStackData data) {
		int javaCallId = 0;
		Object v = data.pop(0);
		if (v != null && v instanceof Number) {
			javaCallId = ((Number) v).intValue();
		}

		if (javaCallId <= 0) {
			if (log.isDebugEnabled()) {
				log.debug("airesponse({},{}) invalid callbackId", callId, data);
			}
			throw new IllegalArgumentException("invalid callbackId");
		}

		LuaStackData rep = data.copy();
		data.reset();
		AIStack<LuaStackData> stack = stacks.remove(javaCallId);
		if (stack != null) {
			if (log.isDebugEnabled()) {
				log.debug("stack[{}] success", javaCallId);
			}
			AIUtil.safeSuccess(stack, rep);
			data.addBoolean(true);
		} else {
			if (log.isDebugEnabled()) {
				log.debug("miss stack[{}], skip callback", javaCallId);
			}
			data.addBoolean(false);
		}
		return true;
	}

	public void close() {
		List<AIStack<LuaStackData>> tmp = new ArrayList<AIStack<LuaStackData>>(
				stacks.values());
		stacks.clear();
		Exception e = new IllegalStateException("close");
		for (AIStack<LuaStackData> stack : tmp) {
			AIUtil.safeFailure(stack, e);
		}
	}
}

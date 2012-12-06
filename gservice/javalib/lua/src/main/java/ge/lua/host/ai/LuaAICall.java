package ge.lua.host.ai;

import ge.lua.LuaArray;
import ge.lua.LuaStateException;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaApp.Command;
import ge.lua.host.LuaCallWithName;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentSkipListMap;
import java.util.concurrent.TimeoutException;

import bma.common.langutil.ai.AIUtil;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackOne;
import bma.common.langutil.ai.vm.AIVM;
import bma.common.langutil.concurrent.ProcessTimerTask;
import bma.common.langutil.concurrent.TimerManager;
import bma.common.langutil.core.RoundRobinInteger;

public class LuaAICall implements LuaCallWithName {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(LuaAICall.class);

	public static final String METHOD_NAME = "airesponse";

	@Override
	public String getName() {
		return METHOD_NAME;
	}

	protected Map<Integer, AIStack<LuaArray>> stacks = new ConcurrentSkipListMap<Integer, AIStack<LuaArray>>();
	protected RoundRobinInteger callId = new RoundRobinInteger(10000000);

	public int aicall(final AIStack<LuaArray> stack, LuaApp app,
			final String funName, final LuaArray req, long timeout,
			TimerManager tm) {
		final AIStack<LuaArray> callStack = new AIStackOne<LuaArray>(stack);
		final int callId = aicall(callStack, app, funName, req);
		ProcessTimerTask task = TimerManager.delay(new Runnable() {

			@Override
			public void run() {
				if (discard(callId)) {
					callStack.failure(new TimeoutException("timeout LuaAICall-"
							+ callId));
				}
			}
		}, timeout);
		AIVM.checkMainTimer(tm).postTimerTask(task);
		return callId;
	}

	public int aicall(final AIStack<LuaArray> stack, LuaApp app,
			final String funName, final LuaArray req) {
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
		if (r && log.isDebugEnabled()) {
			log.debug("discard stack[{}] => {}", callId, r);
		}
		return r;
	}

	@Override
	public boolean call(LuaApp app, int callId, LuaArray data) {
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

		LuaArray rep = data.copy();
		data.reset();

		AIStack<LuaArray> stack = stacks.remove(javaCallId);
		if (stack != null) {
			Object err = rep.pop(0);
			if (log.isDebugEnabled()) {
				log.debug("stack[{}] {}", err == null ? "success" : "fail",
						javaCallId);
			}
			if (err != null) {
				AIUtil.safeFailure(stack, new LuaStateException(err.toString()));
			} else {
				AIUtil.safeSuccess(stack, rep);
			}
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
		List<AIStack<LuaArray>> tmp = new ArrayList<AIStack<LuaArray>>(
				stacks.values());
		stacks.clear();
		Exception e = new IllegalStateException("close");
		for (AIStack<LuaArray> stack : tmp) {
			AIUtil.safeFailure(stack, e);
		}
	}
}

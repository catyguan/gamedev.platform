package ge.lua.appcommand;

import ge.lua.LuaArray;
import ge.lua.service.impl.AccessId;
import ge.lua.service.thrift.TLuaAppCallResult;
import ge.lua.service.thrift.TLuaAppHostManager4AI;

import java.util.ArrayList;
import java.util.List;

import org.apache.thrift.TException;

import bma.common.json.JsonUtil;
import bma.common.langutil.ai.AIUtil;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.ExceptionUtil;
import bma.common.langutil.core.SizeUtil;
import bma.common.langutil.core.SizeUtil.Unit;
import bma.common.langutil.core.ValueUtil;
import bma.common.langutil.io.HostPort;
import bma.common.thrift.ThriftClientConfig;
import bma.common.thrift.ai.AIThriftClient;
import bma.common.thrift.ai.AIThriftClientFactory;

public class AppCommandClient {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(AppCommandClient.class);

	protected AIThriftClientFactory factory;
	protected Dispatcher dispatcher;
	protected int frameMaxLength;
	protected int timeout = 30 * 1000;

	public int getTimeout() {
		return timeout;
	}

	public void setTimeout(int timeout) {
		this.timeout = timeout;
	}

	public int getFrameMaxLength() {
		return frameMaxLength;
	}

	public void setFrameMaxLength(int frameBuffer) {
		this.frameMaxLength = frameBuffer;
	}

	public void setFrameSize(String sz) {
		try {
			this.frameMaxLength = (int) SizeUtil.convert(sz, 1024, Unit.B);
		} catch (Exception e) {
			throw ExceptionUtil.throwRuntime(e);
		}
	}

	public AIThriftClientFactory getFactory() {
		return factory;
	}

	public void setFactory(AIThriftClientFactory factory) {
		this.factory = factory;
	}

	public Dispatcher getDispatcher() {
		return dispatcher;
	}

	public void setDispatcher(Dispatcher dispatcher) {
		this.dispatcher = dispatcher;
	}

	public boolean execute(AIStack<String> stack, final String accessId,
			final String sceneName, final String commandName,
			final LuaArray params, final int tm) {
		if (accessId == null)
			throw new NullPointerException("accessId");
		final AccessId aid = new AccessId(accessId);
		if (aid.isGlobalName()) {
			if (dispatcher != null) {
				AIStackStep<String, String> step = new AIStackStep<String, String>(
						stack) {

					@Override
					protected boolean next(String result) {
						if (result == null) {
							String msg = "dispatch[" + accessId + "] fail";
							if (log.isDebugEnabled()) {
								log.debug(msg);
							}
							return AIUtil.safeFailure(delegate(),
									new IllegalArgumentException(msg));
						}
						return execute(delegate(), result, sceneName,
								commandName, params, tm);
					}
				};
				return dispatcher.find(step, aid.getName());
			}
		}
		if (!aid.isValidId()) {
			String msg = "invalid accessId[" + accessId + "]";
			if (log.isDebugEnabled()) {
				log.debug(msg);
			}
			return AIUtil.safeFailure(stack, new IllegalArgumentException(msg));
		}

		AIStackStep<AIThriftClient, String> step1 = new AIStackStep<AIThriftClient, String>(
				stack) {

			@Override
			protected boolean next(final AIThriftClient client) {

				AIStackStep<TLuaAppCallResult, String> step2 = new AIStackStep<TLuaAppCallResult, String>(
						delegate()) {

					protected void closeClient() {
						try {
							client.close();
						} catch (Exception e) {
						}
					}

					@Override
					protected boolean next(TLuaAppCallResult result) {
						closeClient();
						if (log.isDebugEnabled()) {
							log.debug("AppCommand.execute({},{},{},{}) => {}",
									new Object[] { aid, sceneName, commandName,
											params, result.getResult() });
						}
						return successForward(result.getResult());
					}

					@Override
					public boolean failure(Throwable t) {
						closeClient();
						return super.failure(t);
					}

				};

				try {
					TLuaAppHostManager4AI.Client obj = client
							.createAIObject(TLuaAppHostManager4AI.Client.class);

					List cparams = new ArrayList(2 + params.size());
					cparams.add(sceneName);
					cparams.add(commandName);
					cparams.addAll(params.toList());

					return obj.appAICall(
							step2,
							aid.getAppId(),
							"appCommand",
							JsonUtil.getDefaultMapper().writeValueAsString(
									cparams), tm <= 0 ? timeout : tm);

				} catch (Exception e) {
					return AIUtil.safeFailure(step2, e);
				}

			}
		};

		ThriftClientConfig cfg = new ThriftClientConfig();
		cfg.setFrameMaxLength(frameMaxLength);
		cfg.setHost(new HostPort(aid.getIp(), aid.getPort()));
		cfg.setModule(ValueUtil.empty(aid.getModule()) ? "lua" : aid
				.getModule());
		try {
			return factory.createThriftClient(step1, cfg);
		} catch (TException e) {
			return AIUtil.safeFailure(stack, e);
		}
	}
}

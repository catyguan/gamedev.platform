package ge.lua.httpagent;

import ge.lua.appcommand.AppCommandClient;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.jboss.netty.buffer.ChannelBuffers;
import org.jboss.netty.handler.codec.http.DefaultHttpResponse;
import org.jboss.netty.handler.codec.http.HttpHeaders;
import org.jboss.netty.handler.codec.http.HttpHeaders.Names;
import org.jboss.netty.handler.codec.http.HttpRequest;
import org.jboss.netty.handler.codec.http.HttpResponse;
import org.jboss.netty.handler.codec.http.HttpResponseStatus;
import org.jboss.netty.util.CharsetUtil;

import bma.common.json.JsonUtil;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackReentrant;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.ai.vm.AIContextThread;
import bma.common.langutil.core.ValueUtil;
import bma.common.netty.webserver.RequestContext;
import bma.common.netty.webserver.RequestProcessor;
import bma.common.netty.webserver.SessionManager;
import bma.common.netty.webserver.WebServerUtil;

public abstract class AgentRequestProcessor implements RequestProcessor {

	protected SessionManager sessionManager;
	protected AppCommandClient commandClient;

	public AppCommandClient getCommandClient() {
		return commandClient;
	}

	public void setCommandClient(AppCommandClient commandClient) {
		this.commandClient = commandClient;
	}

	public SessionManager getSessionManager() {
		return sessionManager;
	}

	public void setSessionManager(SessionManager sessionManager) {
		this.sessionManager = sessionManager;
	}

	public Map<String, Object> success(Object result) {
		Map<String, Object> r = new HashMap<String, Object>();
		r.put("success", true);
		if (result != null) {
			r.put("result", result);
		}
		return r;
	}

	public Map<String, Object> error(String code, String msg) {
		Map<String, Object> r = new HashMap<String, Object>();
		r.put("error", true);
		if (code != null) {
			r.put("code", code);
		}
		if (msg != null) {
			r.put("message", msg);
		}
		return r;
	}

	public Map<String, Object> mistake(String code, String msg) {
		Map<String, Object> r = new HashMap<String, Object>();
		r.put("mistake", true);
		if (code != null) {
			r.put("code", code);
		}
		if (msg != null) {
			r.put("message", msg);
		}
		return r;
	}

	public Map<String, Object> paramInvalid(String name) {
		return mistake("param.invalid", name);
	}

	public HttpResponse createJsonResponse(HttpRequest request, String content) {
		if (content == null)
			content = "null";
		HttpResponse response = new DefaultHttpResponse(
				request.getProtocolVersion(), HttpResponseStatus.OK);
		response.setContent(ChannelBuffers.copiedBuffer(content,
				CharsetUtil.UTF_8));
		HttpHeaders.setContentLength(response, response.getContent()
				.readableBytes());
		response.setHeader(Names.CONTENT_TYPE, "application/json;charset=UTF-8");
		return response;
	}

	public HttpResponse createResponse(HttpRequest request, Object obj)
			throws Exception {
		String content = JsonUtil.getDefaultMapper().writeValueAsString(obj);
		return createJsonResponse(request, content);
	}

	public boolean writeResponse(AIStack<Boolean> stack, RequestContext ctx,
			HttpResponse respone) {
		try {
			WebServerUtil.writeAndKeepAlive("Agent", ctx, respone);
			return stack.success(true);
		} catch (Exception e) {
			return stack.failure(e);
		}
	}

	public boolean sendResponse(AIStack<Boolean> stack, RequestContext ctx,
			Object obj) {
		try {
			HttpResponse resp = createResponse(ctx.getRequest(), obj);
			return writeResponse(stack, ctx, resp);
		} catch (Exception e) {
			return stack.failure(e);
		}
	}

	public String checkSessionId(AIStack<Boolean> stack, RequestContext ctx) {
		String sid = sessionManager.getSessionId(ctx.getRequest());
		if (ValueUtil.empty(sid)) {
			sendResponse(stack, ctx,
					mistake("session.invalid", "invalid sessionId"));
			return null;
		}
		return sid;
	}

	public boolean isSuccess(Map r) {
		if (r.containsKey("success")) {
			return ValueUtil.toBoolean(r.get("success"), false);
		}
		return false;
	}

	public boolean execute(AIStack<Map> stack, String accessId,
			String caseName, String methodName,
			Map<String, Object> sessionData, List<Object> params, int tm)
			throws Exception {

		AIStackStep<String, Map> step = new AIStackStep<String, Map>(stack) {

			@Override
			protected boolean next(String result) {
				try {
					List r = JsonUtil.getDefaultMapper().readValue(result,
							List.class);
					return successForward(r.isEmpty() ? null : (Map) r.get(0));
				} catch (Exception e) {
					return super.failure(e);
				}
			}
		};

		String strParams = null;
		if (params != null) {
			strParams = JsonUtil.getDefaultMapper().writeValueAsString(params);
		}
		return execute(step, accessId, caseName, methodName, sessionData,
				strParams, tm);
	}

	public boolean execute(AIStack<String> stack, String accessId,
			String caseName, String methodName,
			Map<String, Object> sessionData, String params, int tm)
			throws Exception {

		String strSession = null;
		if (sessionData != null) {
			strSession = JsonUtil.getDefaultMapper().writeValueAsString(
					sessionData);
		}
		AIStackReentrant<String> stackReturn = new AIStackReentrant<String>(
				stack, AIContextThread.create());
		return commandClient.execute(stackReturn, null, accessId, caseName,
				methodName, strSession, params, tm);
	}
}

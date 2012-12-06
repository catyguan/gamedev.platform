package ge.lua.httpagent;

import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import org.jboss.netty.handler.codec.http.HttpResponse;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.ValueUtil;
import bma.common.netty.webserver.RequestContext;
import bma.common.netty.webserver.RequestForm;
import bma.common.netty.webserver.Session;
import bma.common.netty.webserver.WebServerUtil;

public class ARPProxy extends AgentRequestProcessor {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ARPProxy.class);

	protected List<String> blackList;
	protected List<String> whiteList;

	public List<String> getBlackList() {
		return blackList;
	}

	public void setBlackList(List<String> blockList) {
		this.blackList = blockList;
	}

	public List<String> getWhiteList() {
		return whiteList;
	}

	public void setWhiteList(List<String> whiteList) {
		this.whiteList = whiteList;
	}

	@Override
	public boolean process(AIStack<Boolean> stack, final RequestContext ctx)
			throws Exception {
		final String sid = checkSessionId(stack, ctx);
		if (sid == null)
			return true;

		Session s = sessionManager.getSession(sid, false);
		if (s == null) {
			sendResponse(stack, ctx,
					mistake("token.invalid", "invalid session"));
			return true;
		}
		String token = ValueUtil.toString(s.getAttribute("token"), null);
		String authId = ValueUtil.toString(s.getAttribute("authId"), null);

		if (ValueUtil.empty(token)) {
			sendResponse(stack, ctx, mistake("token.invalid", "invalid token"));
			return true;
		}

		RequestForm form = WebServerUtil.getForm(ctx);
		final String accessId = form.requestValue("aid");
		final String caseName = form.requestValue("c");
		final String methodName = form.requestValue("m");
		final String params = form.requestValue("p");

		if (ValueUtil.empty(accessId)) {
			return sendResponse(stack, ctx, paramInvalid("aid"));
		}
		if (ValueUtil.empty(caseName)) {
			return sendResponse(stack, ctx, paramInvalid("c"));
		}
		if (ValueUtil.empty(methodName)) {
			return sendResponse(stack, ctx, paramInvalid("m"));
		}

		if (blackList != null) {
			if (blackList.contains(caseName)) {
				return sendResponse(
						stack,
						ctx,
						mistake("case.refuse", "case '" + caseName
								+ "' in black list"));
			}
		}
		if (whiteList != null && !whiteList.isEmpty()) {
			if (!whiteList.contains(caseName)) {
				return sendResponse(
						stack,
						ctx,
						mistake("case.refuse", "case '" + caseName
								+ "' not in white list"));
			}
		}

		AIStackStep<String, Boolean> step = new AIStackStep<String, Boolean>(
				stack) {

			@Override
			protected boolean next(String r) {
				HttpResponse resp = createJsonResponse(ctx.getRequest(), r);
				return writeResponse(delegate(), ctx, resp);
			}
		};

		if (log.isDebugEnabled()) {
			log.debug("execute - proxy({},{},{})", new Object[] { accessId,
					caseName, methodName });
		}
		Map<String, Object> sessionData = new TreeMap<String, Object>();
		sessionData.put("token", token);
		sessionData.put("authId", authId);

		return execute(step, accessId, caseName, methodName, sessionData,
				params, 0);
	}
}

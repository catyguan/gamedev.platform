package ge.lua.httpagent;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.ValueUtil;
import bma.common.netty.webserver.RequestContext;
import bma.common.netty.webserver.Session;

public class ARPLogout extends AgentRequestProcessor {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ARPLogout.class);

	protected String accessIdOfAuth;
	protected String caseOfAuth = "auth";
	protected String methodOfAuth = "logout";

	public String getAccessIdOfAuth() {
		return accessIdOfAuth;
	}

	public void setAccessIdOfAuth(String authAccessId) {
		this.accessIdOfAuth = authAccessId;
	}

	public ARPLogout() {
		super();
	}

	public String getCaseOfAuth() {
		return caseOfAuth;
	}

	public void setCaseOfAuth(String caseOfAuth) {
		this.caseOfAuth = caseOfAuth;
	}

	public String getMethodOfAuth() {
		return methodOfAuth;
	}

	public void setMethodOfAuth(String methodOfAuth) {
		this.methodOfAuth = methodOfAuth;
	}

	@Override
	public boolean process(AIStack<Boolean> stack, final RequestContext ctx)
			throws Exception {
		final String sid = checkSessionId(stack, ctx);
		if (sid == null)
			return true;

		Session s = sessionManager.getSession(sid, false);
		final String token = s == null ? null : ValueUtil.toString(
				s.getAttribute("token"), null);
		if (token == null) {
			return sendResponse(stack, ctx, success(true));
		}

		List<Object> params = new ArrayList<Object>(1);
		params.add(token);

		AIStackStep<Map, Boolean> step = new AIStackStep<Map, Boolean>(stack) {

			@Override
			protected boolean next(Map r) {
				if (isSuccess(r)) {
					if (log.isDebugEnabled()) {
						log.debug("logout({}) success", token);
					}
					Session s = sessionManager.getSession(sid, false);
					if (s != null) {
						s.removeAttribute("token");
						s.removeAttribute("authId");
					}
					r = ARPLogout.this.success(true);
				}
				sendResponse(delegate(), ctx, r);
				return true;
			}
		};

		if (log.isDebugEnabled()) {
			log.debug("execute - logout({})", token);
		}
		return execute(step, accessIdOfAuth, caseOfAuth, methodOfAuth, null,
				params, 0);
	}
}

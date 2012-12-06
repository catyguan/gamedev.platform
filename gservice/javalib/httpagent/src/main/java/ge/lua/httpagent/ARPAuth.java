package ge.lua.httpagent;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.ValueUtil;
import bma.common.netty.webserver.RequestContext;
import bma.common.netty.webserver.RequestForm;
import bma.common.netty.webserver.Session;
import bma.common.netty.webserver.WebServerUtil;

public class ARPAuth extends AgentRequestProcessor {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ARPAuth.class);

	protected String accessIdOfAuth;
	protected String caseOfAuth = "auth";
	protected String methodOfAuth = "login";

	public String getAccessIdOfAuth() {
		return accessIdOfAuth;
	}

	public void setAccessIdOfAuth(String authAccessId) {
		this.accessIdOfAuth = authAccessId;
	}

	public ARPAuth() {
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

	protected boolean check(AIStack<Boolean> stack, RequestContext ctx,
			String sid) {
		Session s = sessionManager.getSession(sid, false);
		if (s != null) {
			Object token = s.getAttribute("token");
			if (token != null) {
				return sendResponse(stack, ctx, success(true));
			}
		}
		return false;
	}

	@Override
	public boolean process(AIStack<Boolean> stack, final RequestContext ctx)
			throws Exception {
		final String sid = checkSessionId(stack, ctx);
		if (sid == null)
			return true;

		if (check(stack, ctx, sid)) {
			return true;
		}

		RequestForm form = WebServerUtil.getForm(ctx);
		final String account = form.requestValue("account");
		String passwd = form.requestValue("password");
		
		if (ValueUtil.empty(account)) {
			return sendResponse(stack, ctx, paramInvalid("account"));
		}
		if (ValueUtil.empty(passwd)) {
			return sendResponse(stack, ctx, paramInvalid("password"));
		}

		List<Object> params = new ArrayList<Object>(2);
		params.add(account);
		params.add(passwd);

		AIStackStep<Map, Boolean> step = new AIStackStep<Map, Boolean>(stack) {

			@Override
			protected boolean next(Map r) {
				if (check(delegate(), ctx, sid)) {
					return true;
				}
				if (isSuccess(r)) {
					if (log.isDebugEnabled()) {
						log.debug("auth({}) success", account);
					}
					Object res = r.get("result");
					if (res != null && res instanceof Map) {
						Map result = (Map) res;
						Session s = sessionManager.getSession(sid, true);
						s.setAttribute("token", result.get("token"));
						s.setAttribute("authId", result.get("authId"));
						if (log.isDebugEnabled()) {
							log.debug("login done => {}-{},{}",
									new Object[] { sid, result.get("token"),
											result.get("authId") });
						}
						r = ARPAuth.this.success(true);
					} else {
						r = error("protocol.unknow", "miss result");
					}
				}
				sendResponse(delegate(), ctx, r);
				return true;
			}
		};

		if (log.isDebugEnabled()) {
			log.debug("execute - auth({},#####)", account);
		}
		return execute(step, accessIdOfAuth, caseOfAuth, methodOfAuth, null,
				params, 0);
	}
}

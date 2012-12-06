package ge.lua.httpagent;

import org.jboss.netty.handler.codec.http.HttpRequest;
import org.jboss.netty.handler.codec.http.HttpResponse;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.core.ValueUtil;
import bma.common.netty.webserver.RequestContext;

public class ARPEntry extends AgentRequestProcessor {

	public ARPEntry() {
		super();
	}

	@Override
	public boolean process(AIStack<Boolean> stack, RequestContext ctx)
			throws Exception {
		HttpRequest request = ctx.getRequest();
		String sid = sessionManager.getSessionId(request);
		HttpResponse response = createResponse(request, success(true));
		if (ValueUtil.empty(sid)) {
			sid = sessionManager.createSessionId();
			sessionManager.setSessionId(response, sid);
		}
		return writeResponse(stack, ctx, response);		
	}
}

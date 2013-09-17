package ge.gsupgrade.processor;

import ge.gsupgrade.UpgradeFileSystem;
import ge.gsupgrade.actions.UpgradeAction;
import ge.gsupgrade.actions.UpgradeAction4End;

import java.util.List;

import org.jboss.netty.buffer.ChannelBuffers;
import org.jboss.netty.channel.Channel;
import org.jboss.netty.handler.codec.http.DefaultHttpResponse;
import org.jboss.netty.handler.codec.http.HttpHeaders;
import org.jboss.netty.handler.codec.http.HttpResponse;
import org.jboss.netty.handler.codec.http.HttpResponseStatus;
import org.jboss.netty.handler.codec.http.HttpVersion;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.netty.webserver.RequestContext;
import bma.common.netty.webserver.RequestForm;
import bma.common.netty.webserver.RequestProcessor;
import bma.common.netty.webserver.WebServerUtil;

public class Query implements RequestProcessor {

	private UpgradeFileSystem fileSystem;
	private String url;
	private String host;

	public Query(UpgradeFileSystem fileSystem, String url, String host) {
		super();
		this.fileSystem = fileSystem;
		this.url = url;
		this.host = host;
	}

	public boolean process(AIStack<Boolean> stack, RequestContext ctx)
			throws Exception {
		long endTime = System.currentTimeMillis();
		RequestForm form = WebServerUtil.getForm(ctx);
		String version = form.getValue("version");
		List<UpgradeAction> list = fileSystem.query(url, host, version);
		UpgradeAction4End endAction = new UpgradeAction4End();
		endAction.setVersion(Long.toString(endTime));
		list.add(endAction);

		StringBuilder sb = new StringBuilder();
		for (UpgradeAction a : list) {
			if (sb.length() > 0)
				sb.append("\n");
			a.build(sb);
		}
		byte[] buf = sb.toString().getBytes("UTF-8");

		HttpResponse response = new DefaultHttpResponse(HttpVersion.HTTP_1_1,
				HttpResponseStatus.OK);
		HttpHeaders.setContentLength(response, buf.length);
		response.setContent(ChannelBuffers.copiedBuffer(buf));

		Channel ch = ctx.getChannel();
		ch.write(response);

		WebServerUtil.endWith200("upgrade.query", ctx);

		return stack.success(true);
	}
}

package ge.gsupgrade.processor;

import ge.gsupgrade.UpgradeFileSystem;

import org.jboss.netty.buffer.ChannelBuffers;
import org.jboss.netty.channel.Channel;
import org.jboss.netty.handler.codec.http.DefaultHttpResponse;
import org.jboss.netty.handler.codec.http.HttpHeaders;
import org.jboss.netty.handler.codec.http.HttpResponse;
import org.jboss.netty.handler.codec.http.HttpResponseStatus;
import org.jboss.netty.handler.codec.http.HttpVersion;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.core.ValueUtil;
import bma.common.netty.webserver.RequestContext;
import bma.common.netty.webserver.RequestForm;
import bma.common.netty.webserver.RequestProcessor;
import bma.common.netty.webserver.WebServerUtil;

public class Download implements RequestProcessor {

	private UpgradeFileSystem fileSystem;

	public Download(UpgradeFileSystem fileSystem) {
		super();
		this.fileSystem = fileSystem;
	}

	public boolean process(AIStack<Boolean> stack, RequestContext ctx)
			throws Exception {
		RequestForm form = WebServerUtil.getForm(ctx);
		String id = form.getValue("id");
		String fileName = form.getValue("f");
		byte[] buf = null;
		if (ValueUtil.notEmpty(id) && ValueUtil.notEmpty(fileName)) {
			buf = this.fileSystem.readFile(id, fileName);
		}
		if (buf == null) {
			WebServerUtil.endWithNotFound("upgrade.download", ctx);
			return stack.success(true);
		}

		HttpResponse response = new DefaultHttpResponse(HttpVersion.HTTP_1_1,
				HttpResponseStatus.OK);
		HttpHeaders.setContentLength(response, buf.length);
		response.setContent(ChannelBuffers.copiedBuffer(buf));

		Channel ch = ctx.getChannel();
		ch.write(response);

		WebServerUtil.endWith200("upgrade.download", ctx);

		return stack.success(true);
	}
}

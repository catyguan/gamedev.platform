package ge.lua.httpagent;

import ge.lua.appcommand.AppCommandClient;
import ge.lua.appcommand.Dispatcher;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.vm.AIVM;
import bma.common.langutil.core.ObjectUtil;
import bma.common.langutil.core.StringUtil;
import bma.common.langutil.io.IOUtil;
import bma.common.langutil.log.LogbackUtil;
import bma.common.netty.client.NettyClientBootstrap;
import bma.common.netty.pool.NettyChannelPool;
import bma.common.netty.pool.SimpleNettyChannelPoolFactory;
import bma.common.netty.thrift.client.AIThriftClientFactoryNettyPool;
import bma.common.netty.webserver.NettyWebServer;
import bma.common.netty.webserver.ai.SessionFactory4AI;
import bma.common.netty.webserver.ai.SessionManager4AI;
import bma.common.netty.webserver.common.RequestDispatcherVPath;
import bma.common.netty.webserver.file.RequestProcessorStaticFile;

public class HttpAgentTC {

	AIVM main;
	NettyChannelPool pool;

	@Before
	public void setUp() throws Exception {
		LogbackUtil.setLevel("bma.common.langutil.concurrent.ProcessTimeWheel",
				"INFO");

		main = new AIVM();
		main.initMain();

		NettyClientBootstrap b = new NettyClientBootstrap();
		b.init();

		SimpleNettyChannelPoolFactory pfac = new SimpleNettyChannelPoolFactory();
		pfac.setBootstrap(b);

		pool = new NettyChannelPool();
		pool.setFactory(pfac);
	}

	@After
	public void takeDown() {
		pool.close();
		main.close();
	}

	public NettyWebServer server() throws Exception {
		NettyWebServer s = new NettyWebServer();
		s.setPort(8080);
		s.setName("webServer");
		s.setTraceBufferSize(1024);

		if (s != null) {
			RequestProcessorStaticFile htmlLibs = new RequestProcessorStaticFile();
			htmlLibs.setRoot(IOUtil.getUserDirFile("../../../html.libs")
					.getCanonicalPath());

			RequestDispatcherVPath vpath = new RequestDispatcherVPath(
					"/html.libs/", htmlLibs);

			s.addDispatcher(vpath);
		}

		SessionManager4AI sm = new SessionManager4AI();
		sm.setMaxSessionDataSize(256);
		sm.setSessionDataSize(128);
		sm.setSessionTimeout(3600 * 1000);
		sm.setFactory(new SessionFactory4AI());

		AIThriftClientFactoryNettyPool fac = new AIThriftClientFactoryNettyPool();
		fac.setPool(pool);

		AppCommandClient client = new AppCommandClient();
		client.setFactory(fac);
		client.setFrameSize("10M");
		client.setDispatcher(new Dispatcher() {

			@Override
			public boolean find(AIStack<String> stack, String name) {
				if (StringUtil.equals(name, "test")) {
					return stack.success("test-127.0.0.1-9098-lua-app1");
				}
				return stack.success(null);
			}
		});

		if (s != null) {
			ARPGroup p4ai = new ARPGroup();
			p4ai.setSessionManager(sm);
			p4ai.setClient(client);
			p4ai.setAccessIdOfAuth("test");
			p4ai.init();

			RequestDispatcherVPath vpath = new RequestDispatcherVPath(
					"/agent/", p4ai);
			s.addDispatcher(vpath);
		}

		return s;
	}

	@Test
	public void httpagent_server() throws Exception {
		NettyWebServer s = server();
		s.start();
		ObjectUtil.waitFor(this, 3600 * 1000);
		s.close();
	}

}

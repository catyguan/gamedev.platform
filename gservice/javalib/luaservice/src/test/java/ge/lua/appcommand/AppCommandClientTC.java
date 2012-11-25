package ge.lua.appcommand;

import ge.lua.LuaArray;
import ge.lua.appcommand.AppCommandClient;

import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.stack.AIStackSimple;
import bma.common.langutil.log.LogbackUtil;
import bma.common.netty.client.NettyClientBootstrap;
import bma.common.netty.pool.NettyChannelPool;
import bma.common.netty.pool.SimpleNettyChannelPoolFactory;
import bma.siteone.netty.thrift.client.AIThriftClientFactoryNettyPool;

public class AppCommandClientTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(AppCommandClientTC.class);

	@Before
	public void setUp() {
		LogbackUtil.setLevel("bma.common.langutil.concurrent.ProcessTimeWheel",
				"INFO");
	}

	private AIThriftClientFactoryNettyPool factory() throws Exception {

		NettyClientBootstrap b = new NettyClientBootstrap();
		b.init();

		SimpleNettyChannelPoolFactory pfac = new SimpleNettyChannelPoolFactory();
		pfac.setBootstrap(b);

		NettyChannelPool pool = new NettyChannelPool();
		pool.setFactory(pfac);

		AIThriftClientFactoryNettyPool fac = new AIThriftClientFactoryNettyPool();
		fac.setPool(pool);

		return fac;
	}

	@Test
	public void hello_say() throws Exception {
		AIThriftClientFactoryNettyPool fac = factory();

		try {
			AppCommandClient cl = new AppCommandClient();
			cl.setFactory(fac);
			cl.setFrameSize("10M");

			LuaArray params = new LuaArray();
			params.addString("world");
			AIStackSimple<String> stack = new AIStackSimple<String>(null);
			cl.execute(stack, null,"test-127.0.0.1-9098-lua-app1", "hello", "say",
					params, 0);
			log.info("result => {}", stack.get());

		} catch (Exception e) {
			log.error("error", e);
			e.printStackTrace();
		} finally {
			fac.getPool().close();
		}
	}
}

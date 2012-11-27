package ge.lua.hostservice.gcommand;

import ge.lua.LuaArray;
import ge.lua.appcommand.AppCommandClient;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.core.ObjectUtil;
import bma.common.netty.client.NettyClientBootstrap;
import bma.common.netty.pool.NettyChannelPool;
import bma.common.netty.pool.SimpleNettyChannelPoolFactory;
import bma.common.netty.thrift.client.AIThriftClientFactoryNettyPool;

public class ServiceTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ServiceTC.class);

	NettyChannelPool pool;

	@Before
	public void setupMain() throws Exception {

		NettyClientBootstrap b = new NettyClientBootstrap();
		b.init();

		SimpleNettyChannelPoolFactory pfac = new SimpleNettyChannelPoolFactory();
		pfac.setBootstrap(b);

		pool = new NettyChannelPool();
		pool.setFactory(pfac);

		
	}

	@After
	public void closeMain() {
		pool.close();
	}

	@Test
	public void gcommand_execute() throws Exception {
		AIThriftClientFactoryNettyPool fac = new AIThriftClientFactoryNettyPool();
		fac.setPool(pool);
		
		AppCommandClient client = new AppCommandClient();
		client.setFactory(fac);
		client.setFrameSize("10m");
		
		ServiceLuaCall s = new ServiceLuaCall();
		s.setClient(client);

		if (s != null) {
			LuaArray params = new LuaArray();
			params.addString("world");
			
			LuaArray data = new LuaArray();
			data.addString("test-127.0.0.1-9098-lua-app1");
			data.addString("hello");
			data.addString("say");
			data.addArray(params);

			s.call(null, 1, data);
		}

		ObjectUtil.waitFor(this, 5 * 1000);
	}

}

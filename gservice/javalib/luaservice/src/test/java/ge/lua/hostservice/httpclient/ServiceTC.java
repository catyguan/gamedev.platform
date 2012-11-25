package ge.lua.hostservice.httpclient;

import ge.lua.LuaArray;
import ge.lua.hostservice.httpclient.ServiceLuaCall;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.core.ObjectUtil;
import bma.common.netty.client.NettyClientBootstrap;
import bma.common.netty.pool.NettyChannelPool;
import bma.common.netty.pool.SimpleNettyChannelPoolFactory;

public class ServiceTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ServiceTC.class);

	NettyChannelPool pool;

	@Before
	public void setupMain() {
		SimpleNettyChannelPoolFactory fac = new SimpleNettyChannelPoolFactory();
		fac.setBootstrap(new NettyClientBootstrap().init());
		
		pool = new NettyChannelPool();
		pool.setFactory(fac);
	}

	@After
	public void closeMain() {
		pool.close();
	}

	@Test
	public void httpInvoke() throws Exception {
		ServiceLuaCall s = new ServiceLuaCall();
		s.setPool(pool);

		LuaArray data = new LuaArray();
		data.addString("http://cn.bing.com");
		s.call(null, 1, data);
		
		ObjectUtil.waitFor(this, 60*30*1000);
	}

}

package ge.lua.service;

import ge.lua.service.thrift.TLuaAppCallResult;
import ge.lua.service.thrift.TLuaAppHostManager;

import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.boot.AIServerBoot;
import bma.common.langutil.log.LogbackUtil;
import bma.common.thrift.ThriftClient;
import bma.common.thrift.ThriftClientConfig;
import bma.common.thrift.common.ThriftClientFactoryCommon;
import bma.siteone.netty.thrift.common.TShutdown;

public class LuaAppHostThriftTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(LuaAppHostThriftTC.class);

	@Before
	public void setUp() {
		LogbackUtil.setLevel("bma.common.langutil.concurrent.ProcessTimeWheel",
				"INFO");
	}

	@Test
	public void luaService() {
		System.setProperty("spring_server_xml",
				"classpath:ge/lua/service/lua-service.xml");
		AIServerBoot.main(new String[0]);
	}

	private ThriftClient client(boolean shutdown) throws Exception {
		ThriftClientConfig cfg = new ThriftClientConfig();
		cfg.setFrameMaxLength(1024 * 1024);
		cfg.setHostPort("127.0.0.1:9098");
		cfg.setModule(shutdown ? "shutdown" : "lua");

		ThriftClientFactoryCommon fac = new ThriftClientFactoryCommon();
		return fac.createThriftClient(cfg);
	}

	@Test
	public void shutdown() throws Exception {
		ThriftClient client = client(true);
		try {
			TShutdown.Client obj = client.createObject(TShutdown.Client.class);
			obj.shutdown("");
			log.info("done!!!");
		} finally {
			client.close();
		}
	}

	@Test
	public void client_createApp() throws Exception {
		ThriftClient client = client(false);
		try {
			TLuaAppHostManager.Client obj = client
					.createObject(TLuaAppHostManager.Client.class);
			Object r = obj.createApp("app1", "demo1");
			log.info("createApp = {}", r);
		} catch (Exception e) {
			log.error("error", e);
			e.printStackTrace();
		} finally {
			client.close();
		}
	}

	@Test
	public void client_restartApp() throws Exception {
		ThriftClient client = client(false);
		try {
			TLuaAppHostManager.Client obj = client
					.createObject(TLuaAppHostManager.Client.class);
			Object r = obj.restartApp("app1");
			log.info("restartApp = {}", r);
		} catch (Exception e) {
			log.error("error", e);
			e.printStackTrace();
		} finally {
			client.close();
		}
	}

	@Test
	public void client_closeApp() throws Exception {
		ThriftClient client = client(false);
		try {
			TLuaAppHostManager.Client obj = client
					.createObject(TLuaAppHostManager.Client.class);
			Object r = obj.closeApp("app1");
			log.info("closeApp = {}", r);
		} catch (Exception e) {
			log.error("error", e);
			e.printStackTrace();
		} finally {
			client.close();
		}
	}

	@Test
	public void client_test1() throws Exception {
		ThriftClient client = client(false);
		try {
			TLuaAppHostManager.Client obj = client
					.createObject(TLuaAppHostManager.Client.class);

			if (obj != null) {
				String params = "[\"app.demo1.test1\"]";
				TLuaAppCallResult r = obj.appCall("app1", "require", params);
				log.info("appCall(require) = {}", r.getResult());
			}
			if (obj != null) {
				String params = "[]";
				TLuaAppCallResult r = obj.appAICall("app1", "testHttpClient",
						params, 5000);
				log.info("appCall(testHttpClient) = {}", r.getResult());
			}
		} catch (Exception e) {
			log.error("error", e);
			e.printStackTrace();
		} finally {
			client.close();
		}
	}
}

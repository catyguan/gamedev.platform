
package ge.lua.matrix;

import ge.lua.service.thrift.TLuaAppCallResult;
import ge.lua.service.thrift.TLuaAppHostManager;

import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.log.LogbackUtil;
import bma.common.thrift.ThriftClient;
import bma.common.thrift.ThriftClientConfig;
import bma.common.thrift.common.ThriftClientFactoryCommon;

public class StatefulMatrixTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(StatefulMatrixTC.class);

	@Before
	public void setUp() {
		LogbackUtil.setLevel("bma.common.langutil.concurrent.ProcessTimeWheel",
				"INFO");
	}

	private ThriftClient client() throws Exception {
		ThriftClientConfig cfg = new ThriftClientConfig();
		cfg.setFrameMaxLength(1024 * 1024);
		cfg.setHostPort("127.0.0.1:9098");
		cfg.setModule("lua");

		ThriftClientFactoryCommon fac = new ThriftClientFactoryCommon();
		return fac.createThriftClient(cfg);
	}

	@Test
	public void matrix_testcase() throws Exception {
		ThriftClient client = client();
		try {
			TLuaAppHostManager.Client obj = client
					.createObject(TLuaAppHostManager.Client.class);

			if (obj != null) {
				String params = "[\"bma.matrix.StatefulMatrix_testcase\"]";
				TLuaAppCallResult r = obj.appCall("app1", "include", params);
				log.info("appCall(include) = {}", r.getResult());
			}			
		} catch (Exception e) {
			log.error("error", e);
			e.printStackTrace();
		} finally {
			client.close();
		}
	}

}

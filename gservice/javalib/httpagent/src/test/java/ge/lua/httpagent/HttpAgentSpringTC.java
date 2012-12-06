package ge.lua.httpagent;

import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.boot.AIServerBoot;
import bma.common.langutil.log.LogbackUtil;

public class HttpAgentSpringTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(HttpAgentSpringTC.class);

	@Before
	public void setUp() {
		LogbackUtil.setLevel("bma.common.langutil.concurrent.ProcessTimeWheel",
				"INFO");
	}

	@Test
	public void boot() {
		System.setProperty("spring_server_xml",
				"classpath:ge/lua/httpagent/lua-httpagent.xml");
		AIServerBoot.main(new String[0]);
	}
}

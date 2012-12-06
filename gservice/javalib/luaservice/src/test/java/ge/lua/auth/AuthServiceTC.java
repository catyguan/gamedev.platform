package ge.lua.auth;

import ge.lua.TCBase;

import org.junit.Test;

public class AuthServiceTC extends TCBase {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(AuthServiceTC.class);

	@Test
	public void auth_testcase() throws Exception {
		runLuaTestcase("cloud.auth.auth_testcase");
	}

}

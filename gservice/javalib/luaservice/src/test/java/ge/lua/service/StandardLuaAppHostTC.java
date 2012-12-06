package ge.lua.service;

import ge.lua.host.LuaAppHost;
import ge.lua.host.impl.LuaAppFactorySimple;

import java.io.File;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.vm.AIVM;
import bma.common.langutil.concurrent.TimerManager;
import bma.common.langutil.core.ObjectUtil;
import bma.common.langutil.io.IOUtil;

public class StandardLuaAppHostTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(StandardLuaAppHostTC.class);

	AIVM main;
	TimerManager timer;

	@Before
	public void setupMain() throws Exception {
		main = new AIVM();
		main.setThreadSize(3);
		main.initMain();

		timer = main.sureTimer();
	}

	@After
	public void closeMain() {
		main.close();
	}

	public LuaAppHost host() {
		LuaAppHost r = new LuaAppHost();

		LuaAppFactorySimple fac = new LuaAppFactorySimple();
		fac.addPath(IOUtil.getUserDirFile("docs").toString());
		File platformDir = new File("D:/workroom/gamedev.platform/lua.libs");
		fac.addPath(new File(platformDir, "src").getPath());
		fac.addPath(new File(platformDir, "test").getPath());

		fac.addBootstrap("bma.app.api.Startup");
		fac.setShutdown("bma.app.Application.shutdown");

		r.setFactory(fac);
		r.setVm(main);

		r.init();
		return r;
	}

	@Test
	public void startstop() {
		LuaAppHost m = host();
		log.info("run");
		ObjectUtil.waitFor(this, 1000);
		m.close();
		log.info("end");
	}

}

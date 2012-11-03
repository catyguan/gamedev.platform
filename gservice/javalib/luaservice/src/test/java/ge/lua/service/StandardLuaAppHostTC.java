package ge.lua.service;

import ge.lua.host.LuaAppHost;
import ge.lua.host.impl.LuaAppFactorySimple;

import java.io.File;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.common.AIExecutorJava;
import bma.common.langutil.ai.common.AIExecutorPassive;
import bma.common.langutil.ai.executor.AIExecutor;
import bma.common.langutil.concurrent.TimerManager;
import bma.common.langutil.io.IOUtil;

public class StandardLuaAppHostTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(StandardLuaAppHostTC.class);

	AIExecutorPassive main;
	AIExecutor executor;
	TimerManager timer;

	@Before
	public void setupMain() {
		main = new AIExecutorPassive("main");
		main.setUp();
		main.makeShutdownHook();

		executor = new AIExecutorJava(Executors.newFixedThreadPool(3));
		timer = new TimerManager();
		timer.startManager();

		AIExecutor.setTimerManager(timer);
	}

	@After
	public void closeMain() {
		timer.stopManager();
		main.close(1, TimeUnit.SECONDS);
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
		r.setExecutor(executor);

		r.init();
		return r;
	}

	@Test
	public void startstop() {
		LuaAppHost m = host();
		log.info("run");
		main.run(1000);
		m.close();
		log.info("end");
	}

}

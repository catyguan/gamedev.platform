package ge.lua.host;

import ge.lua.LuaArray;
import ge.lua.host.LuaApp.Command;
import ge.lua.host.ai.LuaAICall;
import ge.lua.host.impl.LuaAppFactorySimple;
import ge.lua.host.loader.LuaLoader;

import java.io.File;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import bma.common.langutil.ai.common.AIExecutorJava;
import bma.common.langutil.ai.common.AIExecutorPassive;
import bma.common.langutil.ai.executor.AIExecutor;
import bma.common.langutil.ai.stack.AIStackSimple;
import bma.common.langutil.concurrent.Event;
import bma.common.langutil.concurrent.TimerManager;
import bma.common.langutil.core.ObjectUtil;
import bma.common.langutil.io.IOUtil;

public class AppHostTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(AppHostTC.class);

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

	public LuaAppHost host1() {
		LuaAppHost r = new LuaAppHost();

		LuaAppFactorySimple fac = new LuaAppFactorySimple();
		fac.addPath(IOUtil.getUserDirFile("docs").toString());
		File platformDir = new File("D:/workroom/gamedev.platform/lua.libs");
		fac.addPath(new File(platformDir, "src").getPath());
		fac.addPath(new File(platformDir, "test").getPath());
		// fac.addBootstrap("test_boot");
		// fac.setShutdown("hostShutdown");

		r.setFactory(fac);
		r.addCall("say", new LuaCall() {

			@Override
			public boolean call(LuaApp app, int callId, LuaArray data) {
				log.info("LuaCall say => {}", data);
				return true;
			}
		});
		r.addCall("asyn", new LuaCall() {

			@Override
			public boolean call(final LuaApp app, final int callId,
					LuaArray data) {
				log.info("LuaCall asyn => {}/{}", data,
						System.identityHashCode(data));
				final LuaArray rdata = data.copy();
				executor.execute(new Runnable() {

					@Override
					public void run() {
						log.info("LuaCall asyn do => {}/{}", rdata,
								System.identityHashCode(rdata));
						app.luaCallResponse(callId, rdata, null);
					}
				});
				return false;
			}
		});
		r.setExecutor(executor);

		r.init();
		return r;
	}

	@Test
	public void startstop() {
		LuaAppHost m = host1();
		log.info("run");
		main.run(1000);
		m.close();
		log.info("end");
	}

	@Test
	public void createGetCloseApp() throws Exception {
		LuaAppHost m = host1();
		try {
			LuaApp app1 = m.createApp("test", "testApp");
			System.out.println("createApp => " + app1);

			LuaApp app2 = m.queryApp("test");
			System.out.println("getApp => " + app2);

			AIStackSimple<Boolean> r3 = new AIStackSimple<Boolean>(null);
			m.closeApp(r3, "test", true);
			System.out.println("closeApp => " + r3.get());

		} finally {
			m.close();
			log.info("end");
		}
	}

	@Test
	public void runLuaTest() throws Exception {
		LuaAppHost m = host1();
		try {
			LuaApp app = m.createApp("test", "test");
			System.out.println("createApp => " + app);

			final Event event = new Event(true);
			app.runCommand(new Command() {

				@Override
				public void process(LuaApp app) {
					try {
						app.require("bma.lang.FlushPrint");
						app.require("bma.host.host_testcase");
					} finally {
						log.info("call end");
						event.setEvent();
					}
				}
			});

			event.waitEvent(3600 * 1000);
			ObjectUtil.waitFor(this, 5000);

		} finally {
			m.close();
			log.info("end");
		}
	}

	@Test
	public void luaLoader() throws Exception {
		LuaAppHost m = host1();
		try {
			LuaApp app = m.createApp("test", "test");
			System.out.println("createApp => " + app);
			final LuaLoader loader = new LuaLoader() {
				@Override
				public Chunk loadChunk(String moduleName) {
					if ("test1".equals(moduleName)) {
						Chunk c = new Chunk();
						c.content = "print(123456)";
						c.name = "embed-test";
						return c;
					}
					return null;
				}
			};
			final Event event = new Event(true);
			app.runCommand(new Command() {

				@Override
				public void process(LuaApp app) {
					try {
						loader.initApp(app);
						app.require("test1");
						app.require("test3");
					} finally {
						log.info("call end");
						event.setEvent();
					}
				}
			});

			event.waitEvent(3600 * 1000);

		} finally {
			m.close();
			log.info("end");
		}
	}

	@Test
	public void aicall() throws Exception {
		LuaAppHost m = host1();
		LuaAICall aicall = new LuaAICall();
		m.addCall(LuaAICall.METHOD_NAME, aicall);

		try {
			LuaApp app = m.createApp("test", "test");

			app.require("bma.lang.FlushPrint");
			app.require("bma.host.javahost");
			app.require("aicall");

			if (app != null) {
				AIStackSimple<LuaArray> stack = new AIStackSimple<LuaArray>(
						null);
				LuaArray req = new LuaArray();
				req.addString("word from java");
				aicall.aicall(stack, app, "aicalllTest", req, 500, timer);

				LuaArray rep = stack.get(5, TimeUnit.SECONDS);
				System.out.println("TestResult ===== "+rep);
			}

			if (app != null) {
				AIStackSimple<LuaArray> stack = new AIStackSimple<LuaArray>(
						null);
				LuaArray req = new LuaArray();
				req.addString("word from java");
				aicall.aicall(stack, app, "aitimeout", req, 1000, timer);
				try {
					LuaArray rep = stack.get(5, TimeUnit.SECONDS);
					System.out.println(rep);
				} catch (Exception e) {
					System.out.println("AICall error - " + e.getMessage());
				}
			}

		} finally {
			m.close();
			log.info("end");
		}
	}
}

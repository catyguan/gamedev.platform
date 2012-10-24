package bma.lua.javalib;

import ge.lua.LuaStackData;
import ge.lua.LuaState;
import ge.lua.LuaStateCallback;
import ge.lua.LuaStateManager;

import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

import org.junit.Test;

import bma.common.langutil.core.ObjectUtil;
import bma.common.langutil.io.IOUtil;

public class LuaBaseTest {

	@Test
	public void base_OpenClose() {
		LuaState L = LuaStateManager.newState();
		System.out.println(L);
		L.close();
	}

	@Test
	public void base_addpath() {
		LuaState L = LuaStateManager.newState();
		System.out.println("" + L.addpath("abc"));
		L.close();
	}

	@Test
	public void base_pcall() {
		LuaState L = LuaStateManager.newState();
		LuaStackData data = new LuaStackData();
		data.addBoolean(true);
		data.addInt(100);
		data.addNull();
		data.addNumber(24.3);
		data.addString("string");
		System.out.println("R:" + L.pcall("print", data) + "-" + data);
		L.close();
	}

	@Test
	public void base_full() {
		LuaState L = LuaStateManager.newState();
		L.addpath(IOUtil.getUserDirFile("docs").toString());
		L.setvar("env", "" + System.identityHashCode(this));

		LuaStackData data = new LuaStackData();
		data.addString("test");
		System.out.println("require >> " + L.pcall("require", data) + "-"
				+ data);

		data.reset();
		System.out.println("mulret >> " + L.pcall("mulret", data) + "-" + data);

		data.reset();
		System.out.println("error >> " + L.pcall("error_fun", data) + "-"
				+ data);

		L.close();
	}

	@Test
	public void base_callback() {
		LuaState L = LuaStateManager.newState();
		L.setCallback(new LuaStateCallback() {

			@Override
			public boolean callback(LuaState state, LuaStackData data) {
				System.out.println("InCallback >> " + state + "," + data);
				data.reset();
				if (data.empty()) {
					data.addString("string from java");
					data.addBoolean(true);
					data.addInt(100);
					data.addNull();
					data.addNumber(24.3);
				} else {
					throw new IllegalArgumentException("error from java");
				}
				return true;
			}
		});
		L.addpath(IOUtil.getUserDirFile("docs").toString());
		LuaStackData data = new LuaStackData();
		data.addString("test2");
		System.out.println("require >> " + L.pcall("require", data));
		L.close();
	}

	@Test
	public void base_multithread() {
		final LuaState L = LuaStateManager.newState();
		L.addpath(IOUtil.getUserDirFile("docs").toString());
		L.setCallback(new LuaStateCallback() {

			@Override
			public boolean callback(LuaState state, LuaStackData data) {
				return true;
			}
		});

		System.out.println(Thread.currentThread());
		Executor exec = Executors.newSingleThreadExecutor();
		Runnable r = new Runnable() {

			@Override
			public void run() {
				System.out.println(Thread.currentThread());
				L.threadInit();
				LuaStackData data = new LuaStackData();
				data.addString("test2");
				System.out.println("require >> " + L.pcall("require", data));
			}
		};
		exec.execute(r);

		ObjectUtil.waitFor(this, 1000);
		L.close();
	}
}

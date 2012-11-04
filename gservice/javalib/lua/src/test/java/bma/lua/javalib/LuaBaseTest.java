package bma.lua.javalib;

import ge.lua.LuaArray;
import ge.lua.LuaState;
import ge.lua.LuaStateCallback;
import ge.lua.LuaStateManager;
import ge.lua.LuaTable;

import java.util.ArrayList;
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
		LuaArray data = buildAllType();
		System.out.println("R:" + L.pcall("print", data) + "-" + data);
		L.close();
	}

	private LuaArray buildAllType() {
		LuaArray data = new LuaArray();
		data.addBoolean(true);
		data.addInt(100);
		data.addNull();
		data.addNumber(24.3);
		data.addString("string");
		LuaTable table = new LuaTable();
		table.putAt("k1", "v1");
		table.putAt("k2", 123);
		table.putAt("k3", true);
		table.putAt("k4", null);
		
		LuaTable t2 = new LuaTable();
		t2.putAt("kk", 234);
		
		LuaArray a1 = new LuaArray();
		a1.addBoolean(true);
		a1.addTable(t2);
		
		table.putAt("k5", a1);
//		
//		a.add(1);
//		a.add(true);
//		table.putAt("k5", a);
		
		data.addTable(table);
		return data;
	}

	@Test
	public void base_eval() {
		LuaState L = LuaStateManager.newState();
		System.out.println("R:" + L.eval("print(1234)", false));
		System.out.println("R:" + L.eval("error2(\"test\")", false));
		L.close();
	}

	@Test
	public void base_full() {
		LuaState L = LuaStateManager.newState();
		L.addpath(IOUtil.getUserDirFile("docs").toString());
		L.setvar("env", "" + System.identityHashCode(this));

		System.out.println("R:" + L.eval("print(\"eval-string\")", false));

		LuaArray data = new LuaArray();
		data.addString("test");
		boolean pr = L.pcall("require", data);
		System.out.println("require >> " + pr + "-" + data);

		data = buildAllType();
		System.out.print("printall >> ");
		System.out.print(L.pcall("printall", data));
		System.out.println("-" + data);

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
			public boolean callback(LuaState state, LuaArray data) {
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
		LuaArray data = new LuaArray();
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
			public boolean callback(LuaState state, LuaArray data) {
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
				LuaArray data = new LuaArray();
				data.addString("test2");
				System.out.println("require >> " + L.pcall("require", data));
			}
		};
		exec.execute(r);

		ObjectUtil.waitFor(this, 1000);
		L.close();
	}
}

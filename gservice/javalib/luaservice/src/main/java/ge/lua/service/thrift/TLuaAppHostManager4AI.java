package ge.lua.service.thrift;

import ge.lua.service.thrift.TLuaAppHostManager.appAICall_args;
import ge.lua.service.thrift.TLuaAppHostManager.appAICall_result;
import ge.lua.service.thrift.TLuaAppHostManager.appCall_args;
import ge.lua.service.thrift.TLuaAppHostManager.appCall_result;
import ge.lua.service.thrift.TLuaAppHostManager.appEval_args;
import ge.lua.service.thrift.TLuaAppHostManager.appEval_result;
import ge.lua.service.thrift.TLuaAppHostManager.closeApp_args;
import ge.lua.service.thrift.TLuaAppHostManager.closeApp_result;
import ge.lua.service.thrift.TLuaAppHostManager.createApp_args;
import ge.lua.service.thrift.TLuaAppHostManager.createApp_result;
import ge.lua.service.thrift.TLuaAppHostManager.getApp_args;
import ge.lua.service.thrift.TLuaAppHostManager.getApp_result;
import ge.lua.service.thrift.TLuaAppHostManager.listApp_args;
import ge.lua.service.thrift.TLuaAppHostManager.listApp_result;
import ge.lua.service.thrift.TLuaAppHostManager.restartApp_args;
import ge.lua.service.thrift.TLuaAppHostManager.restartApp_result;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.thrift.TBase;
import org.apache.thrift.TException;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.thrift.ai.AIBaseStack;
import bma.common.thrift.ai.AIThriftInvoker;
import bma.common.thrift.ai.TAIBaseProcessor;
import bma.common.thrift.ai.TAIBaseServiceClient;
import bma.common.thrift.ai.TAIProcessFunction;

public class TLuaAppHostManager4AI {

	public static interface Iface {

		public boolean createApp(AIStack<Boolean> stack, String appId,
				String appType) throws org.apache.thrift.TException;

		public boolean restartApp(AIStack<Boolean> stack, String appId)
				throws org.apache.thrift.TException;

		public boolean closeApp(AIStack<Boolean> stack, String appId)
				throws org.apache.thrift.TException;

		public boolean getApp(AIStack<TLuaAppInfo> stack, String appId)
				throws org.apache.thrift.TException;

		public boolean listApp(AIStack<List<TLuaAppInfo>> stack)
				throws org.apache.thrift.TException;

		public boolean appCall(AIStack<TLuaAppCallResult> stack, String appId,
				String name, String params) throws org.apache.thrift.TException;

		public boolean appAICall(AIStack<TLuaAppCallResult> stack,
				String appId, String name, String params, int timeout)
				throws org.apache.thrift.TException;

		public boolean appEval(AIStack<Boolean> stack, String appId,
				String content) throws org.apache.thrift.TException;

	}

	public static class Processor<IFACE extends Iface> extends
			TAIBaseProcessor<IFACE> {

		public Processor(IFACE iface) {
			super(
					iface,
					getProcessMap(new HashMap<String, TAIProcessFunction<IFACE, ? extends TBase>>()));
		}

		private static <I extends Iface> Map<String, TAIProcessFunction<I, ? extends org.apache.thrift.TBase>> getProcessMap(
				Map<String, TAIProcessFunction<I, ? extends org.apache.thrift.TBase>> processMap) {
			processMap.put("createApp", new createApp());
			processMap.put("restartApp", new restartApp());
			processMap.put("closeApp", new closeApp());
			processMap.put("getApp", new getApp());
			processMap.put("listApp", new listApp());
			processMap.put("appCall", new appCall());
			processMap.put("appAICall", new appAICall());
			processMap.put("appEval", new appEval());
			return processMap;
		}

		private static class createApp<I extends Iface> extends
				TAIProcessFunction<I, createApp_args> {
			public createApp() {
				super("createApp");
			}

			@Override
			protected createApp_args getEmptyArgsInstance() {
				return new createApp_args();
			}

			protected boolean getResult(AIStack<TBase> stack, I iface,
					createApp_args args) throws org.apache.thrift.TException {
				createApp_result result = new createApp_result();
				return iface.createApp(new AIBaseStack<Boolean>(stack, result),
						args.getAppId(), args.getAppType());
			};

		}

		private static class restartApp<I extends Iface> extends
				TAIProcessFunction<I, restartApp_args> {
			public restartApp() {
				super("restartApp");
			}

			@Override
			protected restartApp_args getEmptyArgsInstance() {
				return new restartApp_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					restartApp_args args) throws org.apache.thrift.TException {
				restartApp_result result = new restartApp_result();
				return iface.restartApp(
						new AIBaseStack<Boolean>(stack, result),
						args.getAppId());
			}
		}

		private static class closeApp<I extends Iface> extends
				TAIProcessFunction<I, closeApp_args> {
			public closeApp() {
				super("closeApp");
			}

			@Override
			protected closeApp_args getEmptyArgsInstance() {
				return new closeApp_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					closeApp_args args) throws org.apache.thrift.TException {
				closeApp_result result = new closeApp_result();
				return iface.closeApp(new AIBaseStack<Boolean>(stack, result),
						args.getAppId());
			}
		}

		private static class getApp<I extends Iface> extends
				TAIProcessFunction<I, getApp_args> {
			public getApp() {
				super("getApp");
			}

			@Override
			protected getApp_args getEmptyArgsInstance() {
				return new getApp_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					getApp_args args) throws org.apache.thrift.TException {
				getApp_result result = new getApp_result();
				return iface.getApp(
						new AIBaseStack<TLuaAppInfo>(stack, result),
						args.getAppId());
			}
		}

		private static class listApp<I extends Iface> extends
				TAIProcessFunction<I, listApp_args> {
			public listApp() {
				super("listApp");
			}

			@Override
			protected listApp_args getEmptyArgsInstance() {
				return new listApp_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					listApp_args args) throws org.apache.thrift.TException {
				listApp_result result = new listApp_result();
				return iface.listApp(new AIBaseStack<List<TLuaAppInfo>>(stack,
						result));
			}
		}

		private static class appAICall<I extends Iface> extends
				TAIProcessFunction<I, appAICall_args> {
			public appAICall() {
				super("appAICall");
			}

			@Override
			protected appAICall_args getEmptyArgsInstance() {
				return new appAICall_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					appAICall_args args) throws org.apache.thrift.TException {
				appAICall_result result = new appAICall_result();
				return iface.appAICall(new AIBaseStack<TLuaAppCallResult>(
						stack, result), args.getAppId(), args.getName(), args
						.getParams(), args.getTimeout());
			}
		}

		private static class appCall<I extends Iface> extends
				TAIProcessFunction<I, appCall_args> {
			public appCall() {
				super("appCall");
			}

			@Override
			protected appCall_args getEmptyArgsInstance() {
				return new appCall_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					appCall_args args) throws org.apache.thrift.TException {
				appCall_result result = new appCall_result();
				return iface.appCall(new AIBaseStack<TLuaAppCallResult>(stack,
						result), args.getAppId(), args.getName(), args
						.getParams());
			}
		}

		private static class appEval<I extends Iface> extends
				TAIProcessFunction<I, appEval_args> {
			public appEval() {
				super("appEval");
			}

			@Override
			protected appEval_args getEmptyArgsInstance() {
				return new appEval_args();
			}

			@Override
			protected boolean getResult(AIStack<TBase> stack, I iface,
					appEval_args args) throws org.apache.thrift.TException {
				appEval_result result = new appEval_result();
				return iface.appEval(new AIBaseStack<Boolean>(stack, result),
						args.getAppId(), args.getContent());
			}
		}
	}

	public static class Client extends TAIBaseServiceClient implements Iface {

		public Client(AIThriftInvoker invoker) {
			super(invoker);
		}

		@Override
		public boolean createApp(AIStack<Boolean> stack, String appId,
				String appType) throws TException {
			return super.invoke(stack, new createApp_result(), "createApp",
					new createApp_args(appId, appType));
		}

		@Override
		public boolean restartApp(AIStack<Boolean> stack, String appId)
				throws TException {
			return super.invoke(stack, new restartApp_result(), "restartApp",
					new restartApp_args(appId));
		}

		@Override
		public boolean closeApp(AIStack<Boolean> stack, String appId)
				throws TException {
			return super.invoke(stack, new closeApp_result(), "closeApp",
					new closeApp_args(appId));
		}

		@Override
		public boolean getApp(AIStack<TLuaAppInfo> stack, String appId)
				throws TException {
			return super.invoke(stack, new getApp_result(), "getApp",
					new getApp_args(appId));
		}

		@Override
		public boolean listApp(AIStack<List<TLuaAppInfo>> stack)
				throws TException {
			return super.invoke(stack, new listApp_result(), "listApp",
					new listApp_args());
		}

		@Override
		public boolean appCall(AIStack<TLuaAppCallResult> stack, String appId,
				String name, String params) throws TException {
			return super.invoke(stack, new appCall_result(), "appCall",
					new appCall_args(appId, name, params));
		}

		@Override
		public boolean appAICall(AIStack<TLuaAppCallResult> stack,
				String appId, String name, String params, int timeout)
				throws TException {
			return super.invoke(stack, new appAICall_result(), "appAICall",
					new appAICall_args(appId, name, params, timeout));
		}

		@Override
		public boolean appEval(AIStack<Boolean> stack, String appId,
				String content) throws TException {
			return super.invoke(stack, new appEval_result(), "appEval",
					new appEval_args(appId, content));
		}
	}
}

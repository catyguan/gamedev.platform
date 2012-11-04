package ge.lua.host;

import ge.lua.LuaArray;
import ge.lua.LuaState;

import java.util.List;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import bma.common.langutil.ai.stack.AIStack;

public class LuaApp {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(LuaApp.class);

	public static interface Command {
		public void process(LuaApp app);
	}

	private String id;
	private String type;
	private List<String> pathList;
	private List<String> bootstrapList;
	private String shutdown;

	private long createTime;

	// runtime
	protected LuaState state;
	protected LuaAppHost host;
	protected volatile boolean run = false;
	protected Queue<Command> queue = new ConcurrentLinkedQueue<Command>();

	public LuaApp() {
		super();
		this.createTime = System.currentTimeMillis();
	}

	public long getCreateTime() {
		return createTime;
	}

	public void setCreateTime(long createTime) {
		this.createTime = createTime;
	}

	public LuaAppHost getHost() {
		return host;
	}

	public void setHost(LuaAppHost host) {
		this.host = host;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getType() {
		return type;
	}

	public void setType(String name) {
		this.type = name;
	}

	public List<String> getPathList() {
		return pathList;
	}

	public void setPathList(List<String> pathList) {
		this.pathList = pathList;
	}

	public List<String> getBootstrapList() {
		return bootstrapList;
	}

	public void setBootstrapList(List<String> bootstrapList) {
		this.bootstrapList = bootstrapList;
	}

	public String getShutdown() {
		return shutdown;
	}

	public void setShutdown(String shutdown) {
		this.shutdown = shutdown;
	}

	public LuaState getState() {
		return state;
	}

	public void setState(LuaState state) {
		this.state = state;
	}

	public Queue<Command> getQueue() {
		return queue;
	}

	public boolean isRun() {
		return run;
	}

	public void setRun(boolean run) {
		this.run = run;
	}

	public void runCommand(Command command) {
		this.host.runAppCommand(this, command);
	}

	public boolean runCall(final AIStack<LuaArray> stack,
			final String funName, final LuaArray data) {
		runCommand(new Command() {

			@Override
			public void process(LuaApp app) {
				try {
					pcall(funName, data);
					stack.success(data);
				} catch (Exception t) {
					stack.failure(t);
				}
			}
		});
		return false;
	}

	public void luaCallResponse(int callId, LuaArray data, String error) {
		this.host.luaCallResponse(this, callId, data, error);
	}

	public boolean setvar(String key, String value) {
		return state.setvar(key, value);
	}

	public void pcall(String funName, LuaArray data) {
		state.pcall(funName, data, true);
	}

	public void eval(String content) {
		state.eval(content, true);
	}

	void require(String packageName) {
		LuaArray data = new LuaArray();
		data.addString(packageName);
		pcall("require", data);
	}

	@Override
	public String toString() {
		return id + "-" + this.state;
	}
}

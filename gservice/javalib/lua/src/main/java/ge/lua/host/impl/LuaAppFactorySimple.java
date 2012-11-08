package ge.lua.host.impl;

import ge.lua.host.LuaApp;
import ge.lua.host.LuaAppFactory;

import java.util.ArrayList;
import java.util.List;

public class LuaAppFactorySimple implements LuaAppFactory {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(LuaAppFactorySimple.class);

	private List<String> pathList;
	private List<String> bootstrapList;
	private List<String> launchList;
	private String shutdown;

	public String getShutdown() {
		return shutdown;
	}

	public void setShutdown(String shutdown) {
		this.shutdown = shutdown;
	}

	public List<String> getBootstrapList() {
		return bootstrapList;
	}

	public void setBootstrapList(List<String> bootstrapList) {
		this.bootstrapList = bootstrapList;
	}

	public void setBootstrap(String v) {
		this.bootstrapList = new ArrayList<String>(1);
		this.bootstrapList.add(v);
	}

	public void addBootstrap(String v) {
		if (this.bootstrapList == null)
			this.bootstrapList = new ArrayList<String>();
		this.bootstrapList.add(v);
	}

	public List<String> getLaunchList() {
		return launchList;
	}

	public void setLaunchList(List<String> launchList) {
		this.launchList = launchList;
	}

	public void setLaunch(String v) {
		if (this.launchList == null)
			this.launchList = new ArrayList<String>();
		this.launchList.add(v);
	}

	public void addLaunch(String v) {
		if (this.launchList == null)
			this.launchList = new ArrayList<String>();
		this.launchList.add(v);
	}

	public List<String> getPathList() {
		return pathList;
	}

	public void setPathList(List<String> pathList) {
		this.pathList = pathList;
	}

	public void setPath(String v) {
		this.pathList = new ArrayList<String>(1);
		this.pathList.add(v);
	}

	public void addPath(String v) {
		if (this.pathList == null)
			this.pathList = new ArrayList<String>();
		this.pathList.add(v);
	}

	@Override
	public LuaApp createApp(String id, String type) {
		LuaApp app = new LuaApp();
		app.setPathList(this.pathList);
		app.setBootstrapList(this.bootstrapList);
		app.setLaunchList(this.launchList);
		app.setShutdown(this.shutdown);
		return app;
	}

}

package ge.lua.service.impl;

import bma.common.langutil.core.ValueUtil;

public class AccessId {

	private String name;
	private String ip;
	private int port;
	private String module;
	private String appId;

	public AccessId() {
		super();
	}

	public AccessId(String s) {
		super();
		this.create(s);
	}

	public AccessId(String name, String ip, int port, String module,
			String appId) {
		super();
		this.name = name;
		this.ip = ip;
		this.port = port;
		this.module = module;
		this.appId = appId;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getIp() {
		return ip;
	}

	public void setIp(String ip) {
		this.ip = ip;
	}

	public int getPort() {
		return port;
	}

	public void setPort(int port) {
		this.port = port;
	}

	public String getModule() {
		return module;
	}

	public void setModule(String module) {
		this.module = module;
	}

	public String getAppId() {
		return appId;
	}

	public void setAppId(String appId) {
		this.appId = appId;
	}

	public void create(String s) {
		String[] plist = s.split("-", 5);
		this.name = plist[0];
		if (plist.length > 1) {
			this.ip = plist[1];
		}
		if (plist.length > 2) {
			this.port = ValueUtil.intValue(plist[2], 0);
		}
		if (plist.length > 3) {
			this.module = plist[3];
		}
		if (plist.length > 4) {
			this.appId = plist[4];
		}
	}

	public boolean isGlobalName() {
		return ValueUtil.empty(ip);
	}

	public boolean isValidId() {
		return ValueUtil.notEmpty(ip) && port > 0;
	}

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder(256);
		sb.append(name == null ? "" : name).append('-');
		sb.append(ip == null ? "" : ip).append("-");
		sb.append(port).append('-');
		sb.append(module == null ? "" : module).append('-');
		if (appId != null) {
			sb.append(appId);
		}
		return sb.toString();
	}
}

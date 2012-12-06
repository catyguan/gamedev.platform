package ge.lua.httpagent;

import ge.lua.appcommand.AppCommandClient;

import java.util.ArrayList;
import java.util.List;

import bma.common.netty.webserver.SessionManager;
import bma.common.netty.webserver.ai.RequestProcess4AI;
import bma.common.netty.webserver.common.RequestDispatchGroup;

public class ARPGroup extends RequestProcess4AI {

	protected SessionManager sessionManager;
	protected AppCommandClient client;
	protected String accessIdOfAuth;
	protected List<String> blackList;
	protected List<String> whiteList;

	public String getAccessIdOfAuth() {
		return accessIdOfAuth;
	}

	public void setAccessIdOfAuth(String accessIdOfAuth) {
		this.accessIdOfAuth = accessIdOfAuth;
	}

	public SessionManager getSessionManager() {
		return sessionManager;
	}

	public void setSessionManager(SessionManager sessionManager) {
		this.sessionManager = sessionManager;
	}

	public AppCommandClient getClient() {
		return client;
	}

	public void setClient(AppCommandClient client) {
		this.client = client;
	}

	public void addWhite(String caseName) {
		if (this.whiteList == null) {
			this.whiteList = new ArrayList<String>();
		}
		this.whiteList.add(caseName);
	}

	public List<String> getWhiteList() {
		return whiteList;
	}

	public void setWhiteList(List<String> blockList) {
		this.whiteList = blockList;
	}

	public void addBlack(String caseName) {
		if (this.blackList == null) {
			this.blackList = new ArrayList<String>();
		}
		this.blackList.add(caseName);
	}

	public List<String> getBlackList() {
		return blackList;
	}

	public void setBlackList(List<String> blackList) {
		this.blackList = blackList;
	}

	public void init() {
		SessionManager sm = sessionManager;

		RequestDispatchGroup rd = new RequestDispatchGroup();

		if (rd != null) {
			ARPEntry p = new ARPEntry();
			p.setSessionManager(sm);
			p.setCommandClient(client);
			rd.add("/entry", p);
		}

		if (rd != null) {
			ARPAuth p = new ARPAuth();
			p.setSessionManager(sm);
			p.setCommandClient(client);
			p.setAccessIdOfAuth(accessIdOfAuth);
			rd.add("/auth", p);
		}

		if (rd != null) {
			ARPLogout p = new ARPLogout();
			p.setSessionManager(sm);
			p.setCommandClient(client);
			p.setAccessIdOfAuth(accessIdOfAuth);
			rd.add("/logout", p);
		}

		if (rd != null) {
			ARPProxy p = new ARPProxy();
			p.setSessionManager(sm);
			p.setCommandClient(client);
			p.setWhiteList(whiteList);
			p.setBlackList(blackList);
			rd.add("/proxy", p);
		}

		this.setSessionRouter(sm);
		this.setProcessor(rd);

	}
}

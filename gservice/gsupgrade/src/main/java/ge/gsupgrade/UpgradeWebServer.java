package ge.gsupgrade;

import ge.gsupgrade.processor.Download;
import ge.gsupgrade.processor.Query;

import java.util.HashMap;
import java.util.Map;

import bma.common.netty.webserver.NettyWebServer;
import bma.common.netty.webserver.RequestProcessor;
import bma.common.netty.webserver.common.RequestDispatchGroup;

public class UpgradeWebServer extends NettyWebServer {

	private UpgradeFileSystem fileSystem;
	private String accessUrl;
	private String accessHost;

	public UpgradeWebServer() {
		super();
		setName("upgradeWebServer");
	}

	public String getAccessUrl() {
		return accessUrl;
	}

	public void setAccessUrl(String accessUrl) {
		this.accessUrl = accessUrl;
	}

	public String getAccessHost() {
		return accessHost;
	}

	public void setAccessHost(String accessHost) {
		this.accessHost = accessHost;
	}

	public UpgradeFileSystem getFileSystem() {
		return fileSystem;
	}

	public void setFileSystem(UpgradeFileSystem fileSystem) {
		this.fileSystem = fileSystem;
	}

	public void init() {
		RequestDispatchGroup root = new RequestDispatchGroup();
		Map<String, RequestProcessor> ps = new HashMap<String, RequestProcessor>();

		Query p1 = new Query(fileSystem, accessUrl, accessHost);
		ps.put("/query", p1);

		Download p2 = new Download(fileSystem);
		ps.put("/update", p2);

		root.setProcessors(ps);
		addDispatcher(root);
	}

}

package ge.gsupgrade;

import junit.framework.TestCase;
import bma.common.langutil.core.ObjectUtil;
import bma.common.netty.webserver.NettyWebServer;

public class UpgradeWebServerTC extends TestCase {

	public UpgradeFileSystem fs() {
		PathSetting pset;
		UpgradeFileSystem r = new UpgradeFileSystem();

		pset = new PathSetting();
		pset.setFileSystemType(UpgradeFileSystem.fsLua);
		pset.setId("bmacore");
		pset.setPath("D:\\workroom\\gamedev.platform\\lua.libs\\src");
		r.addPath(pset);

		return r;
	}

	public NettyWebServer server() {
		UpgradeWebServer s = new UpgradeWebServer();
		s.setPort(8080);
		s.setTraceBufferSize(1024);
		s.setAccessUrl("http://localhost:8080/update");
		s.setFileSystem(fs());
		s.init();
		return s;
	}

	public void test_server() {
		NettyWebServer s = server();
		s.start();
		ObjectUtil.waitFor(this, 60 * 1000);
		s.close();
	}

}

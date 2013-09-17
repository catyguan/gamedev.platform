package ge.gsupgrade;

import ge.gsupgrade.actions.UpgradeAction;

import java.util.List;

import junit.framework.TestCase;

public class UpgradeFileSystemTC extends TestCase {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(UpgradeFileSystemTC.class);

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

	public void test_query() {

		UpgradeFileSystem ufs = fs();

		String version = "0";
		List<UpgradeAction> list = ufs.query("http://localhost/", "", version);
		for (UpgradeAction a : list) {
			log.debug(a.toString());
		}
	}

}

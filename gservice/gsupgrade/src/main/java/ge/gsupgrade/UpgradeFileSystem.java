package ge.gsupgrade;

import ge.gsupgrade.actions.UpgradeAction;
import ge.gsupgrade.actions.UpgradeAction4Download;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import bma.common.langutil.core.VPath;
import bma.common.langutil.core.ValueUtil;
import bma.common.langutil.io.IOUtil;

public class UpgradeFileSystem {
	
	public static final int fsAll = 0;
    public static final int fsResource = 1;
	public static final int fsAppData = 2;
    public static final int fsLua = 3;
	public static final int fsTemp = 4;

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(UpgradeFileSystem.class);

	private List<PathSetting> paths;

	public List<PathSetting> getPaths() {
		return paths;
	}

	public void setPaths(List<PathSetting> paths) {
		this.paths = paths;
	}
	
	public void addPath(PathSetting pset) {
		if(this.paths==null)this.paths = new ArrayList<PathSetting>();
		this.paths.add(pset);
	}

	protected void query(File dir, long time, VPath root, List<VPath> result) {
		File[] fs = dir.listFiles();
		if (fs == null)
			return;
		for (File f : fs) {
			if (f.isDirectory() && !f.getName().equals(".")
					&& !f.getName().equals("..")) {
				query(f, time, root.add(f.getName()), result);
			} else {
				if (f.lastModified() > time) {
					VPath fpath = root.add(f.getName());
					result.add(fpath);
				}
			}
		}
	}

	public List<UpgradeAction> query(String purl, String host, String version) {
		if(purl==null)purl="";
		if(purl.indexOf('?')==-1) {
			purl += "?";
		} else {
			purl += "&";
		}
		LinkedList<UpgradeAction> r = new LinkedList<UpgradeAction>();
		long tm = ValueUtil.longValue(version, 0);
		if (paths != null) {
			for (PathSetting pset : paths) {
				File path = new File(pset.getPath());
				if (!path.exists()) {
					if (log.isWarnEnabled()) {
						log.warn("'{}' not exists", pset.getPath());
					}
					continue;
				}
				VPath root = VPath.root();
				LinkedList<VPath> list = new LinkedList<VPath>(); 
				query(path, tm, root, list);
				for (VPath vPath : list) {
					UpgradeAction4Download ua = new UpgradeAction4Download();
					ua.setFileName(vPath.toString(false));					
					ua.setFileSystemType(pset.getFileSystemType());
					ua.setHost(host);
					String url = purl;					
					url+="id="+pset.getId()+"&f="+vPath.toString(false);
					ua.setUrl(url);
					r.add(ua);
				}
			}
		}
		return r;
	}
	
	public byte[] readFile(String id, String vpath) throws IOException {
		if(paths==null)return null;
		for (PathSetting pset : paths) {
			if(pset.getId().equals(id)) {
				String file = pset.fileName(vpath);
				return IOUtil.readFileToBytes(new File(file));
			}
		}
		return null;
	}
}

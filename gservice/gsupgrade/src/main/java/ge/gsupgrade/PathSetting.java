package ge.gsupgrade;

import bma.common.langutil.core.VPath;

public class PathSetting {

	private String id;
	private int fileSystemType;
	private String path;

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public int getFileSystemType() {
		return fileSystemType;
	}

	public void setFileSystemType(int fileSystemType) {
		this.fileSystemType = fileSystemType;
	}

	public String getPath() {
		return path;
	}

	public void setPath(String path) {
		this.path = path;
	}

	public String fileName(String fname) {
		VPath p = VPath.create(fname);
		return p.bindRootFile(this.path).getPath();
	}
}

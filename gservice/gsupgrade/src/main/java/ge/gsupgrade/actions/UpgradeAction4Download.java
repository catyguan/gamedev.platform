package ge.gsupgrade.actions;

import bma.common.langutil.core.ValueUtil;

public class UpgradeAction4Download extends UpgradeAction {

	private String url;
	private String host;
	private int fileSystemType;
	private String fileName;
	
	public UpgradeAction4Download() {
		setCommand("dl");
	}

	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

	public String getHost() {
		return host;
	}

	public void setHost(String host) {
		this.host = host;
	}

	public int getFileSystemType() {
		return fileSystemType;
	}

	public void setFileSystemType(int fileSystemType) {
		this.fileSystemType = fileSystemType;
	}

	public String getFileName() {
		return fileName;
	}

	public void setFileName(String fileName) {
		this.fileName = fileName;
	}

	@Override
	public void buildParam(StringBuilder sb) {
		sb.append("l=").append(this.url);
		sb.append(",").append("t=").append(this.fileSystemType);
		sb.append(",").append("f=").append(this.fileName);
		if(ValueUtil.notEmpty(this.host)) {
			sb.append(",").append("h=").append(this.host);
		}
	}

}

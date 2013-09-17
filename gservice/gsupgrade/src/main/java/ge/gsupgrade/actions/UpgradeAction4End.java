package ge.gsupgrade.actions;

import bma.common.langutil.core.ValueUtil;

public class UpgradeAction4End extends UpgradeAction {

	private String version;
	
	public UpgradeAction4End() {
		setCommand("end");
	}

	public String getVersion() {
		return version;
	}

	public void setVersion(String url) {
		this.version = url;
	}

	@Override
	public void buildParam(StringBuilder sb) {
		if(ValueUtil.notEmpty(this.version)) {
			sb.append("v=").append(this.version);
		}
	}

}

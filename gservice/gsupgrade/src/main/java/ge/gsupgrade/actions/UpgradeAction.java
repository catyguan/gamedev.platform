package ge.gsupgrade.actions;

public abstract class UpgradeAction {

	private String command;

	public String getCommand() {
		return command;
	}

	protected void setCommand(String command) {
		this.command = command;
	}
	
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		build(sb);
		return sb.toString();
	}
	
	public void build(StringBuilder sb) {
		sb.append(this.command).append(':');
		buildParam(sb);
	}

	public abstract void buildParam(StringBuilder sb);
}

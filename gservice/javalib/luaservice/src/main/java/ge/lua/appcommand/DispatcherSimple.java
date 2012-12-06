package ge.lua.appcommand;

import java.util.Map;

import bma.common.langutil.ai.stack.AIStack;

public class DispatcherSimple implements Dispatcher {

	private Map<String, String> accessIdMap;

	public Map<String, String> getAccessIdMap() {
		return accessIdMap;
	}

	public void setAccessIdMap(Map<String, String> accessIds) {
		this.accessIdMap = accessIds;
	}

	@Override
	public boolean find(AIStack<String> stack, String name) {
		if (this.accessIdMap != null) {
			return stack.success(this.accessIdMap.get(name));
		}
		return stack.success(null);
	}

}

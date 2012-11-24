package ge.lua.appcommand;

import bma.common.langutil.ai.stack.AIStack;

public interface Dispatcher {

	public boolean find(AIStack<String> stack, String name);
}

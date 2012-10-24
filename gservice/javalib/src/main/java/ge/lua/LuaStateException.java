package ge.lua;

public class LuaStateException extends RuntimeException {

	public LuaStateException() {
		super();
	}

	public LuaStateException(String message, Throwable cause) {
		super(message, cause);
	}

	public LuaStateException(String message) {
		super(message);
	}

	public LuaStateException(Throwable cause) {
		super(cause);
	}

}

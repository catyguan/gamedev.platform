package ge.lua;

import bma.common.langutil.core.ValueUtil;

public abstract class LuaProxy {

	public abstract int getSize();

	public abstract int getType(int idx);

	public abstract Object getAt(int idx);

	public abstract String nextKey();

	public abstract int getType(String key);

	public abstract Object getAt(String key);

	public abstract void reset();

	public abstract void pushValue(Object v);

	public static int asInt(Object v) {
		if (v == null)
			return 0;
		if (v instanceof Number) {
			return ((Number) v).intValue();
		}
		return ValueUtil.intValue(v.toString(), 0);
	}

	public static boolean asBoolean(Object v) {
		if (v == null)
			return false;
		if (v instanceof Boolean) {
			return ((Boolean) v).booleanValue();
		}
		return ValueUtil.booleanValue(v.toString(), false);
	}

	public static double asNumber(Object v) {
		if (v == null)
			return 0;
		if (v instanceof Number) {
			return ((Number) v).doubleValue();
		}
		return ValueUtil.doubleValue(v.toString(), 0);
	}

	public static String asString(Object v) {
		if (v == null)
			return null;
		return v.toString();
	}

	public static Object create(boolean table) {
		return table ? new LuaTable() : new LuaArray();
	}

	public static Object fromBoolean(boolean v) {
		return v;
	}

	public static Object fromInt(int v) {
		return v;
	}

	public static Object fromNumber(double v) {
		return v;
	}

	public static int type(Object v) {
		if (v == null) {
			return 0;
		}
		if (v instanceof String) {
			return 4;
		}
		if (v instanceof Integer) {
			return -2;
		}
		if (v instanceof Boolean) {
			return 1;
		}
		if (v instanceof Number) {
			return 3;
		}
		if (v instanceof LuaTable) {
			return 5;
		}
		if (v instanceof LuaArray) {
			return -3;
		}
		return 0;
	}

}

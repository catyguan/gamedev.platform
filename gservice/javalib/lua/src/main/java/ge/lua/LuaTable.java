package ge.lua;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public class LuaTable extends LuaProxy {

	public static Object NULL = new Object();
	public static LuaTable EMPTY = new LuaTable();

	private Map<String, Object> data = new TreeMap<String, Object>();
	private Iterator<String> iterator;

	public void putAt(String k, Object v) {
		if (v == null) {
			data.put(k, NULL);
			return;
		}
		data.put(k, v);
	}

	public Map<String, Object> getData() {
		return data;
	}

	@Override
	public int getSize() {
		return data.size();
	}

	@Override
	public int getType(int idx) {
		return 0;
	}

	@Override
	public Object getAt(int idx) {
		return null;
	}

	@Override
	public String nextKey() {
		if (iterator == null) {
			iterator = data.keySet().iterator();
		}
		if (iterator.hasNext()) {
			return iterator.next();
		} else {
			iterator = null;
			return null;
		}
	}

	@Override
	public int getType(String key) {
		return type(getAt(key));
	}

	@Override
	public Object getAt(String key) {
		Object v = data.get(key);
		if (v == NULL)
			v = null;
		return v;
	}

	@Override
	public void reset() {
		data.clear();
	}

	private String key;

	@Override
	public void pushValue(Object v) {
		// System.out.println("PV=" + v);
		if (key == null) {
			key = v == null ? "" : v.toString();
		} else {
			data.put(key, v);
			key = null;
		}
	}

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("T[");
		for (Map.Entry<String, Object> e : data.entrySet()) {
			sb.append(e.getKey()).append("=");
			if (e.getValue() == NULL) {
				sb.append("nil");
			} else {
				sb.append(e.getValue());
			}
			sb.append(";");
		}
		sb.append("");
		sb.append("]");
		return sb.toString();
	}

	public String getString(String key, String def) {
		Object r = data.get(key);
		if (r == null)
			return def;
		return r.toString();
	}

	public static LuaTable checkNull(LuaTable v) {
		return v == null ? EMPTY : v;
	}

	public void bind(Map<?, ?> m) {
		if (m != null && !m.isEmpty()) {
			for (Map.Entry<?, ?> e : m.entrySet()) {
				String k = e.getKey().toString();
				Object o = e.getValue();
				if (o != null) {
					if (o instanceof List) {
						LuaArray la = new LuaArray();
						la.bind((List) o);
						o = la;
					} else if (o instanceof Map) {
						LuaTable lt = new LuaTable();
						lt.bind((Map) o);
						o = lt;
					}
				}
				data.put(k, o);
			}
		}
	}

	public Map toMap() {
		if (!data.isEmpty()) {
			Map<String, Object> r = new HashMap<String, Object>(data.size());
			for (Map.Entry<String, ?> e : data.entrySet()) {
				String k = e.getKey();
				Object o = e.getValue();
				if (o != null) {
					if (o instanceof List) {
						LuaArray la = new LuaArray();
						la.bind((List) o);
						o = la;
					} else if (o instanceof Map) {
						LuaTable lt = new LuaTable();
						lt.bind((Map) o);
						o = lt;
					}
				}
				r.put(k, o);
			}	
			return r;
		}
		return null;
	}
}

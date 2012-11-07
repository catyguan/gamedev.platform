package ge.lua;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import bma.common.langutil.core.ValueUtil;

/**
 * LuaState调用和返回的数据对象
 * 
 * @author guanzhong
 * 
 */
public class LuaArray extends LuaProxy {

	private List<Object> data;

	public int getSize() {
		return data == null ? 0 : data.size();
	}

	public boolean empty() {
		return data == null ? true : data.isEmpty();
	}

	public boolean notEmpty() {
		return !empty();
	}

	public void setSize(int size) {
		this.data = new ArrayList<Object>(size);
	}

	public List<Object> sureData() {
		if (this.data == null)
			this.data = new ArrayList<Object>();
		return this.data;
	}

	public void addNumber(double v) {
		sureData().add(v);
	}

	public void addNumber(int idx, double v) {
		sureData().add(idx, v);
	}

	public void addInt(int v) {
		sureData().add(v);
	}

	public void addInt(int idx, int v) {
		sureData().add(idx, v);
	}

	public void addString(String s) {
		sureData().add(s);
	}

	public void addString(int idx, String s) {
		sureData().add(idx, s);
	}

	public void addTable(LuaTable t) {
		sureData().add(t);
	}

	public void addTable(int idx, LuaTable t) {
		sureData().add(idx, t);
	}

	public void addArray(LuaArray a) {
		sureData().add(a);
	}

	public void addArray(int idx, LuaArray a) {
		sureData().add(idx, a);
	}

	public void addAll(List<?> t) {
		if (t == null || t.isEmpty())
			return;
		List<Object> l = sureData();
		l.addAll(t);
	}

	public void addAll(int idx, List<?> t) {
		if (t == null || t.isEmpty())
			return;
		List<Object> l = sureData();
		l.addAll(idx, t);
	}

	public void addNull() {
		sureData().add(null);
	}

	public void addNull(int idx) {
		sureData().add(idx, null);
	}

	public void addBoolean(boolean v) {
		sureData().add(v);
	}

	public void addBoolean(int idx, boolean v) {
		sureData().add(idx, v);
	}

	public int getType(int idx) {
		Object v = getAt(idx);
		return type(v);
	}

	public Object getAt(int idx) {
		if (data == null)
			return null;
		if (idx < data.size())
			return data.get(idx);
		return null;
	}

	public Object pop(int idx) {
		if (data == null)
			return null;
		if (idx < data.size())
			return data.remove(idx);
		return null;
	}

	public int getInt(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return 0;
		if (v instanceof Number) {
			Number n = (Number) v;
			return n.intValue();
		}
		return ValueUtil.intValue(v.toString(), 0);
	}

	public Number getNumberObject(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return 0;
		if (v instanceof Number) {
			return (Number) v;
		}
		return null;
	}

	public double getNumber(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return 0;
		if (v instanceof Number) {
			Number n = (Number) v;
			return n.doubleValue();
		}
		return ValueUtil.doubleValue(v.toString(), 0);
	}

	public String getString(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return "";
		return v.toString();
	}

	public boolean getBoolean(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return false;
		if (v instanceof Boolean) {
			Boolean n = (Boolean) v;
			return n.booleanValue();
		}
		return ValueUtil.booleanValue(v.toString(), false);
	}

	public LuaTable getTable(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return null;
		if (v instanceof LuaTable) {
			return (LuaTable) v;
		}
		return null;
	}

	public LuaArray getArray(int idx) {
		Object v = getAt(idx);
		if (v == null)
			return null;
		if (v instanceof LuaArray) {
			return (LuaArray) v;
		}
		return null;
	}

	public List<Object> getData() {
		return data;
	}

	public void setData(List<Object> data) {
		this.data = data;
	}

	public void reset() {
		if (this.data != null) {
			this.data.clear();
		}
	}

	public boolean error(String msg) {
		this.reset();
		if (ValueUtil.empty(msg)) {
			msg = "error";
		}
		this.addString(msg);
		return false;
	}

	public LuaArray copy() {
		LuaArray r = new LuaArray();
		r.data = new ArrayList<Object>(this.data);
		return r;
	}

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder(128);
		if (this.data == null) {
			sb.append("<empty>");
		} else {
			sb.append(data.size()).append("[");
			Iterator<Object> it = data.iterator();
			while (it.hasNext()) {
				sb.append(it.next());
				if (it.hasNext())
					sb.append(",");
			}
			sb.append("]");
		}
		return sb.toString();
	}

	@Override
	public void pushValue(Object v) {
		sureData().add(v);
	}

	@Override
	public String nextKey() {
		return null;
	}

	@Override
	public int getType(String key) {
		return 0;
	}

	@Override
	public Object getAt(String key) {
		return null;
	}

	public void bind(List ps) {
		if (ps != null && !ps.isEmpty()) {
			List<Object> l = sureData();
			for (Object o : ps) {
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

				l.add(o);
			}
		}
	}

	public List toList() {
		if (data != null && !data.isEmpty()) {
			List<Object> r = new ArrayList<Object>(data.size());
			for (Object o : data) {
				if (o != null) {
					if (o instanceof LuaArray) {
						LuaArray la = (LuaArray) o;
						o = la.toList();
					} else if (o instanceof LuaTable) {
						LuaTable lt = (LuaTable) o;
						o = lt.toMap();
					}
				}
				r.add(o);
			}
			return r;
		}
		return null;
	}
}

package ge.lua.dal;

import ge.lua.LuaArray;
import ge.lua.LuaTable;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaCallWithName;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Executor;

import org.springframework.dao.DataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCallback;
import org.springframework.jdbc.core.PreparedStatementCreator;

import bma.common.langutil.ai.AIUtil;
import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackROOT;
import bma.common.langutil.core.DateTimeUtil;
import bma.common.langutil.core.ValueUtil;

public class ServiceLuaCall implements LuaCallWithName {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ServiceLuaCall.class);

	public static final String METHOD_NAME = "dal.execute";

	@Override
	public String getName() {
		return METHOD_NAME;
	}

	private Map<String, JdbcTemplate> jdbcTemplates;
	private Executor executor;

	public ServiceLuaCall() {
		super();
	}

	public Executor getExecutor() {
		return executor;
	}

	public void setExecutor(Executor executor) {
		this.executor = executor;
	}

	public Map<String, JdbcTemplate> getJdbcTemplates() {
		return jdbcTemplates;
	}

	public void setJdbcTemplates(Map<String, JdbcTemplate> v) {
		this.jdbcTemplates = v;
	}

	public void setJdbcTemplate(String name, JdbcTemplate jt) {
		if (this.jdbcTemplates == null)
			this.jdbcTemplates = new HashMap<String, JdbcTemplate>();
		this.jdbcTemplates.put(name, jt);
	}

	@Override
	public boolean call(final LuaApp app, final int callId, LuaArray data)
			throws Exception {

		AIStackROOT<LuaArray> root = new AIStackROOT<LuaArray>() {

			@Override
			public boolean end(LuaArray result, Throwable t) {
				if (t != null && log.isDebugEnabled()) {
					log.debug("DAL ServiceLuaCall fail", t);
				}
				if (app != null) {
					app.luaCallResponse(callId, result,
							t == null ? null : t.getMessage());
				} else {
					if (log.isDebugEnabled()) {
						log.debug("app is null,discard {}/{}", result, t);
					}
				}
				return true;
			}
		};

		String dsName = data.getString(0);
		if (ValueUtil.empty(dsName)) {
			throw new IllegalArgumentException("dataSource name invalid");
		}
		String sql = data.getString(1);
		if (ValueUtil.empty(sql)) {
			throw new IllegalArgumentException("sql invalid");
		}
		LuaArray params = data.getArray(2);
		LuaTable options = data.getTable(3);
		return invoke(root, dsName, sql, params, options);
	}

	public boolean invoke(final AIStack<LuaArray> stack, final String dsName,
			final String sql, final LuaArray params, LuaTable options) {
		try {
			final LuaTable opt = LuaTable.checkNull(options);
			if (this.executor == null || opt.getBoolean("syn", false)) {
				LuaArray r = invoke(dsName, sql, params, opt);
				return stack.success(r);
			} else {
				this.executor.execute(new Runnable() {

					@Override
					public void run() {
						try {
							LuaArray result = invoke(dsName, sql, params, opt);
							AIUtil.safeSuccess(stack, result);
						} catch (Exception e) {
							AIUtil.safeFailure(stack, e);
						}
					}
				});
				return false;
			}
		} catch (Exception e) {
			return stack.failure(e);
		}
	}

	protected LuaArray invoke(String dsName, final String sql,
			final LuaArray params, final LuaTable options) throws Exception {
		JdbcTemplate jt = this.jdbcTemplates != null ? this.jdbcTemplates
				.get(dsName) : null;
		if (jt == null) {
			throw new IllegalArgumentException("datasource(" + dsName
					+ ") invalid");
		}

		return jt.execute(new PreparedStatementCreator() {

			@Override
			public PreparedStatement createPreparedStatement(Connection con)
					throws SQLException {
				if (log.isDebugEnabled()) {
					log.debug("execute {}", sql);
				}
				PreparedStatement st = con.prepareStatement(sql);
				if (params != null) {
					int c = params.getSize();
					for (int i = 0; i < c; i++) {
						Object v = params.getAt(i);
						if (v == null) {
							st.setString(i + 1, "null");
						} else if (v instanceof String) {
							String s = (String) v;
							st.setString(i + 1, s);
						} else if (v instanceof Boolean) {
							Boolean s = (Boolean) v;
							st.setBoolean(i + 1, s);
						} else if (v instanceof Integer) {
							Integer s = (Integer) v;
							st.setInt(i + 1, s);
						} else if (v instanceof Number) {
							Number s = (Number) v;
							st.setDouble(i + 1, s.doubleValue());
						} else {
							st.setString(i + 1, v.toString());
						}
					}
				}
				return st;
			}
		}, new PreparedStatementCallback<LuaArray>() {
			@Override
			public LuaArray doInPreparedStatement(PreparedStatement ps)
					throws SQLException, DataAccessException {
				LuaArray r = new LuaArray();
				if (options.getBoolean("update", false)) {
					int c = ps.executeUpdate();
					r.addInt(c);
					return r;
				} else {
					ResultSet rs = ps.executeQuery();
					ResultSetMetaData rsmd = rs.getMetaData();
					int cc = rsmd.getColumnCount();

					while (rs.next()) {
						LuaTable t = new LuaTable();
						for (int i = 0; i < cc; i++) {
							String k = rsmd.getColumnLabel(i + 1);
							Object v = rs.getObject(i + 1);
							if (v != null) {
								if (v instanceof Date) {
									Date d = (Date) v;
									v = DateTimeUtil.formatDateTime(d);
								}
							}
							t.putAt(k, v);
						}
						r.addTable(t);
					}
				}
				return r;
			}
		});
	}
}

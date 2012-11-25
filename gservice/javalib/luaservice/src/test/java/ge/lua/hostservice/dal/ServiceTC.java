package ge.lua.hostservice.dal;

import ge.lua.LuaArray;
import ge.lua.LuaTable;
import ge.lua.hostservice.dal.ServiceLuaCall;

import java.util.Date;
import java.util.concurrent.Executors;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.springframework.jdbc.core.JdbcTemplate;

import bma.common.jdbctemplate.JdbcTemplateUtil;
import bma.common.langutil.core.DateTimeUtil;
import bma.common.langutil.core.ObjectUtil;

import com.mchange.v2.c3p0.ComboPooledDataSource;

public class ServiceTC {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ServiceTC.class);

	ComboPooledDataSource ds;

	@Before
	public void setupMain() throws Exception {

		JdbcTemplateUtil.disableDebug(true);

		ds = new ComboPooledDataSource();
		ds.setDriverClass("com.mysql.jdbc.Driver");
		ds.setJdbcUrl("jdbc:mysql://127.0.0.1:3306/test?user=root&password=root&useUnicode=true&characterEncoding=utf8");
		ds.setInitialPoolSize(10);
		ds.setMinPoolSize(5);
		ds.setMaxPoolSize(30);
		ds.setAcquireIncrement(5);
		ds.setMaxIdleTime(10);
		ds.setMaxStatements(0);
	}

	@After
	public void closeMain() {
		ds.close();
	}

	@Test
	public void dalInvoke() throws Exception {
		ServiceLuaCall s = new ServiceLuaCall();
		s.setExecutor(Executors.newCachedThreadPool());
		s.setJdbcTemplate("test", new JdbcTemplate(ds));

		if (s != null) {
			LuaTable opts = new LuaTable();
			// opts.putAt("syn", true);

			LuaArray data = new LuaArray();
			data.addString("test");
			data.addString("select * from t1");
			data.addNull();
			data.addTable(opts);

			s.call(null, 1, data);
		}

		if (s == null) {
			LuaTable opts = new LuaTable();
			opts.putAt("syn", true);
			opts.putAt("update", true);

			LuaArray params = new LuaArray();
			params.addString("test2");
			params.addNumber(1.2);
			params.addString(DateTimeUtil.formatDateTime(new Date()));

			LuaArray data = new LuaArray();
			data.addString("test");
			data.addString("INSERT INTO t1 VALUES(NULL,?,?,?)");
			data.addArray(params);
			data.addTable(opts);

			s.call(null, 2, data);
		}

		ObjectUtil.waitFor(this, 5 * 1000);
	}

}

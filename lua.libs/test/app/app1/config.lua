-- 调试配置
CONFIG.DEBUG = true

CONFIG.Log = {
	type="print",
	logPattern="#LUA# %date [%category] %level - %message\n",
}

CONFIG.Services = {
	"bma.host.java.Service",
	"bma.dal.host.Service",
	"bma.persistent.mysqlhost.Service",
	"bma.gcommand.host.Service",
}

CONFIG.DAL = {
	DSName = {},
}
CONFIG.DAL.DSName["*"] = "test"

CONFIG.GCommand = {
	GName = {
		test="test-127.0.0.1-9098-lua-app1",
	}
}

CONFIG.Living = {
    heartBeatTime = 1000,
}


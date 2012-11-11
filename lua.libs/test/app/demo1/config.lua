-- 调试配置
CONFIG.DEBUG = true

CONFIG.Log = {
	type="console",
	logPattern="#LUA# %date [%category] %level - %message\n",
}

CONFIG.Services = {
	"bma.host.java.Service",
	"bma.dal.host.Service",
	"bma.state.mysqlhost.Service",		
}

CONFIG.DAL = {
	DSName = {},
}
CONFIG.DAL.DSName["*"] = "test"

CONFIG.Living = {
    heartBeatTime = 1000,
}


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
	"bma.session.local.Service",	
}

CONFIG.DAL = {
	DSName = {},
}
CONFIG.DAL.DSName["*"] = "test"

CONFIG.Living = {
    heartBeatTime = 1000,
}


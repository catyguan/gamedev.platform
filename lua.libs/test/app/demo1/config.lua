-- 调试配置
CONFIG.DEBUG = true

CONFIG.Log = {
	type="console"
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


-- 以下代码为bootstrap.lua的构建代码 --
local Logger = require("bma.lang.Logger")    
LOG = Logger.console()
LOG:setLevel(Logger.INFO)

-- 以下代码为应用的日志代码 --
-- 方法1，直接使用LOG
LOG:debug("test","start debug")

-- 方法2，定义分类日志对象
local log = bma.lang.Logger.create(LOG,"test2")

log:debug("hello %s","world")
log:debug("start debug")
log:info("trying to contact server")
log:warn("server did not responded yet")
log:error("server unreachable")

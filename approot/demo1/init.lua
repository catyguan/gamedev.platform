-- demo1/init.lua
-- bootstrap.lua

-- avoid memory leak
collectgarbage("setpause", 100)
collectgarbage("setstepmul", 5000)

-- init
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")
    
require("demo1.config")
    
-- init Logger
require("bma.lang.Logger")
local Logger = bma.lang.Logger
    
LOG = Logger.luaprint()
LOG:setLevel(Logger.DEBUG)
print("init logger -> LOG") 

-- init Print
require("bma.lang.FilePrint")
local n = GS_APP_ID or "unknow"
local fp = bma.lang.FilePrint.new("e:/logtmp/demo1_"..n..".out")
fp:bind(true)

require("ge.gservice.GService")
    
-- helper
unload = testcase.unload
run = testcase.include
reload = testcase.unload

print("Demo1["..n.."] startup")
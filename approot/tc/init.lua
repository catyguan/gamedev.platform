-- bootstrap.lua

-- avoid memory leak
collectgarbage("setpause", 100)
collectgarbage("setstepmul", 5000)

-- init
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")
    
require("tc.config")
    
-- init Logger
require("bma.lang.Logger")
local Logger = bma.lang.Logger
    
    
LOG = Logger.console()
LOG:setLevel(Logger.DEBUG)
print("init logger -> LOG")    
    
-- helper
unload = testcase.unload
run = testcase.include
reload = testcase.unload

    

-- app/demo1/test1.lua
require("bma.host.AICall")
require("bma.dal.host.Service") 

print("hello","word")

bma.dal.host.Service.install()

function testDAL(callId)
    print("aicall("..callId..") from java")
    
    local HS = class.instance("bma.dal.Service")
    local cb = aicall():response(callId):callback()
    HS:execute(cb, "test", "SELECT * FROM t1")
    return true
    
end

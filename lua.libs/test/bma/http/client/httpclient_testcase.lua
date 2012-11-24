-- app/demo1/test1.lua
require("bma.host.AICall")
require("bma.http.client.host.Service") 

print("hello","word")

bma.http.client.host.Service.install()

function testHttpClient(callId)
    print("aicall("..callId..") from java")
    
    local HS = class.instance("bma.http.client.Service")
    local cb = aicall():response(callId):callback()
    HS:execute(cb, "http://cn.bing.com/404")
    return true
    
end

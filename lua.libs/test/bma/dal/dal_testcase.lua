
local DAL
if true then
	DAL = class.forName("bma.dal.host.Service")
	DAL.install()
end	

local dal = class.instance("bma.dal.Service")

-- select
if true then
    local fun = nil
    fun = function(err, ...)
        print("callback",err, ...)    
    end
    print("call")
    print(HS:call(fun, "say", "hello world"))    
    print("call2")
    print(HS:call({response=fun}, "say", "hello world2"))
    
    print("asyn call")
    print(HS:call(fun, "asyn", "hello world-asyn"))
    
    print("call & discard")
    local msgId = HS:call(fun, "asyn", "hello world2-asyn")    
    HS:discardCallResponse(msgId)
    print("callId = "..msgId)
        
    print("tryCall ")
    print(HS:tryCall(fun, "say", "hello world3"))
    
    if simulate then
    	while true do
    		local r = HS:processCall(function(...)
    			return unpack({...})
    		end)
    		if not r then break end
    	end 
    end
end

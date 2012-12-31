require("bma.test.bootstrap")

CONFIG.C2dxHost = {
	Methods = {
		say = 0,
		asyn = 1,
		fail = 2,
	}
}

local c2dxHS = class.forName("bma.host.c2dx.Service")
c2dxHS.install()

local HS = class.instance("bma.host.Service")

-- call & response
if false then
    local fun = nil
    fun = function(err, ...)
        print("callback",err, ...)    
    end
    print("call")
    HS:call(fun, "say", "hello world")
    print("call2")
    HS:call({handler=fun}, "say", "hello world2")
	
	print("scall")
	local r = HS:scall("say", "hello world3")
	print("scall => ", r)
    
    print("asyn call")
    print("asyn callId = ", HS:call(fun, "asyn", "hello world-asyn"))
    
    print("call & discard")
    local msgId = HS:call(fun, "asyn", "hello world2-asyn")    
    HS:discardCallResponse(msgId)
    print("callId = "..msgId)
        
    print("tryCall ")
    print(HS:tryCall(fun, "say", "hello world3"))
end

-- AICall
if false then
	require("bma.host.AICall")
	
	local fun = function(...)
        print("callback",...)    
    end    
    
    print("asyn call")
    if false then
		local call = aicall("asyn","hello word-aicall")
		call:next(fun):run()
	end
	
	if false then
		local call = aicall("asyn","hello word-aicall")
	    call:next(
	    	aicall("asyn","word-insert"):next(fun)
	    ):run()    
    end
    
    if false then
    	local call = aicall("fail")
    	call:error(function(err) print("error====",err) end):run()
    end
    
    if true then
		local call = aicall("asyn","hello word-aicall")
	    call:error(
	    	function(err)
	    		print("error====",err)
	    	end	    
	    ):next(
	    	aicall("fail","word-insert")
	    ):run()    
    end
end

-- timer
if false then	    
	local timer = function(id)
		return function()
			print("timer", id, os.date())
			-- error("fuck")
			return true
		end		
	end
    
    print("start", os.date(), os.mclock())    
    HS:setFixTimer(timer(1), 5000, 3000)
    HS:setTimer(timer(2), 1000)
    
    local timerId = HS:setTimer(timer(3), 2000)
    HS:removeTimer(timerId)
    
end   

-- CC_FUNCTION_CALL
if false then
	local fun = function(err, f)
        print("callback",err, f)    
		print("function call = ", f("world"))
    end
    print("get function")
    HS:call(fun, "function")
end

-- CCObject
if false then
	local fun = function(err, o)
        print("callback",err, o)    
		print("object method = ", o.visible(false))
    end
    print("get object")
    HS:call(fun, "object")
end

-- CCObject Call
if true then
	local fun = function(err, o)
        print("callback",err, o)    
		print("object call = ", o(false))
    end
    print("get object call")
    HS:call(fun, "ocall")
end
require("bma.test.bootstrap")

local simulate = false

if simulate then
	local simulateHS = class.forName("bma.host.simulate.Service")
	simulateHS.install()
end

local javaHS
if true then
	javaHS = class.forName("bma.host.java.Service")
	javaHS.install()
end	

local HS = class.instance("bma.host.Service")

-- call & response
if false then
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

-- AICall
if true then
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
    local timer = function()
    	print("timer", os.date())
    end
    
    print("start", os.date(), os.mclock())    
    HS:setTimer(timer, 3000)
    HS:setTimer(timer, 1000)
    
    local timerId = HS:setTimer(timer, 2000)
    HS:removeTimer(timerId)
    
    if simulate then
    	while true do
    		local r = HS:processTimer(-1)
    		if not r then break end
    	end 
    end
    
end   

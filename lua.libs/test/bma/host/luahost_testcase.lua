require("bma.test.bootstrap")

if true then
	local HSS = class.forName("bma.host.CommonLuaHostService")
	HSS.install()
end

print(">>> luahost common functions testcase start")
local HS = class.instance("bma.host.Service")

-- invoke & response
if false then
    print("invoke -- say")
    local r = HS:invoke("say", "hello world")
    print("invoke -- response --", r)
end

-- ainvoke & response
if false then
    local fun = nil
    fun = function(err, ...)
        print("ainvoke callback",err, ...)    
    end
    print("ainvoke -- say1")
    HS:ainvoke(fun, "say", "hello world")
    print("ainvoke -- say2")
    HS:ainvoke({handler=fun}, "say", "hello world2")
	
    print("ainvoke -- asyn")
    print("asyn callId = ", HS:ainvoke(fun, "asyn", "hello world-asyn"))
    
    print("ainvoke -- asyn & discard")
    local callId = HS:ainvoke(fun, "asyn", "hello world2-asyn")    
    HS:discardAInvokeResponse(callId)
    print("discard callId = "..callId)
        
    -- print("tryCall ")
    -- print(HS:tryCall(fun, "say", "hello world3"))
end

-- AICall
if false then
	require("bma.host.AICall")
	
	local fun = function(...)
        print("aicall callback",...)    
    end        
    
    if false then
		print("AICall -- asyn call")
		local call = aicall("asyn","hello word-aicall")
		call:next(fun):run()
	end
	
	if false then
		print("AICall -- asyn nested call")
		local call = aicall("asyn","hello word-aicall")
	    call:next(
	    	aicall("asyn","word-insert"):next(fun)
	    ):run()    
    end
    
    if false then
		print("AICall -- asyn fail")
    	local call = aicall("fail")
    	call:error(function(err) print("error====",err) end):run()
    end
    
    if true then
		print("AICall -- asyn nested fail")	
		local call = aicall("asyn","hello word-aicall")
	    call:error(
	    	function(err)
	    		print("error====",err)
	    	end	    
	    ):next(
	    	aicall("fail","error in nested call")
	    ):run()    
    end
end

-- timer
if false then

	local timer = function(id)
		local c = 1
		return function()
			print("timer", id, c, os.date())
			c = c + 1
			if c < 5 then
				-- error("fuck")
				return true
			end
			print("timer",id,"quit")
			return false
		end
	end
    
    print("start", os.date(), os.mclock())    
    HS:setFixTimer(timer(1), 3000, 5000)
    HS:setTimer(timer(2), 1000)
    
    local timerId = HS:setTimer(timer(3), 2000)
    HS:removeTimer(timerId)
    
end

-- LuaFunction to luahost function
if true then
	local fun = function(v)
		-- error("fuck")
		print("luaFunction callback", v)    
	end
	local o,callId = HS:wrapClosure(fun)
	HS:invoke("luaf", o,"wrapClosure")
	
	HS:invoke("luaf", fun, "directFunction")
end

print("<<< luahost common functions testcase end")
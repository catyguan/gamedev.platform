require("bma.test.bootstrap")

if true then
	local c2dxHS = class.forName("bma.host.c2dx.Service")
	c2dxHS.install()
end

local HS = class.instance("bma.host.Service")   

-- CC_FUNCTION_CALL
if false then
	local fun = function(f)
        print("pass",f)    
		print("function call = ", f("world"))
    end
    print("get function")
    local f = HS:invoke("function")
	fun(f)
end

-- CCObject
if false then
	local fun = function(o)
        print("pass",o)    
		print("object method = ", o:visible(false))
    end
    print("get object")
    local f = HS:invoke("object")
	fun(f)
end

-- CCObject Call
if false then
	local fun = function(o)
        print("pass",o)    
		print("object call = ", o(false))
    end
    print("get object call")
    local f = HS:invoke("ocall")
	fun(f)
end

-- CCObject invoke
if false then
	-- ?????? can't run now
	local fun = function(o)
        print("pass",o)    
		print("object invoke = ", o(false))
    end
    print("get object")
    local f = HS:invoke("object")
	fun(f)
end
require("bma.test.bootstrap")

local c2dxHS = class.forName("bma.host.c2dx.Service")
c2dxHS.install()

local HS = class.instance("bma.host.Service")

   

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
		print("object method = ", o:visible(false))
    end
    print("get object")
    HS:call(fun, "object")
end

-- CCObject Call
if false then
	local fun = function(err, o)
        print("callback",err, o)    
		print("object call = ", o(false))
    end
    print("get object call")
    HS:call(fun, "ocall")
end

-- CCObject invoke
if false then
	local fun = function(err, o)
        print("callback",err, o)    
		print("object invoke = ", o(false))
    end
    print("get object")
    HS:call(fun, "object")
end
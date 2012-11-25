
local gcmd = class.instance("bma.gcommand.Service")

-- local
if false then
    local fun = function(err, ...)
        print("callback",err, ...)    
    end
        
    print("call local")
    print(gcmd:command(fun, "local", "hello", "say", "world"))
end


-- hostservice
if false then
    local fun = function(err, ...)
        print("callback",err, ...)    
    end
        
    print("call hostservice")
    print(gcmd:command(fun, "test", "hello", "say", "world"))
end

-- forward
if true then
    local fun = function(err, ...)
        print("callback",err, ...)    
    end
        
    print("call forward")
    print(gcmd:command(fun, "local", "hello", "forward", "world"))
end
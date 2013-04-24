require("bma.test.bootstrap")

if true then
	if not class.hasInstance("bma.host.Service") then
		local c2dxHS = class.forName("bma.host.dotnet.Service")
		dotnetHS.install()
	end
end

local HS = class.instance("bma.host.Service")   

-- Object invoke
if false then
	local fun = function(o)
        print("pass",o)    
		print("object method = ", o:visible(false))
    end
    print("get object")
    local f = HS:invoke("object")
	fun(f)
end

-- Object Call
if true then
	local fun = function(o)
        print("pass",o)    
		print("object call = ", o(false))
    end
    print("get object call")
    local f = HS:invoke("object")
	fun(f)
end

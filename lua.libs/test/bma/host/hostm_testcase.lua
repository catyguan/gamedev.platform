local HS = class.instance("bma.host.Service")

if false then
	local cb = function(err,...)
		print("callback",err,...)
	end
	local r = HS:createApp(cb, "app2","app2")
	print("createApp => ", r)
end

if false then
	local cb = function(err,...)
		print("callback",err,...)
	end
	local r = HS:restartApp(cb, "app1")
	print("restartApp => ", r)
end

if false then
	local cb = function(err,...)
		print("callback",err,...)
	end
	local r = HS:appCall(cb, "app1","print",1,true,"test")
	print("appCall=> ", r)
end  

if true then
	local cb = function(err,...)
		print("callback",err,...)
	end
	local r = HS:closeApp(cb, "app1")
	print("appCall=> ", r)
end  


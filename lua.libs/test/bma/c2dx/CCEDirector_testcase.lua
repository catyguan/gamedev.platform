require("bma.lang.ext.Dump")
require("bma.c2dx.Common")

if true then
	local HS = class.instance("bma.host.Service")
	
	print("Director simple test")
	local s = director:createObject("testScene1", "Hello World 1!!!",Point(240,240));
	print("create = ",s)
	director:pushScene(s)	
	
	HS:setTimer(function()
		local s = director:createObject("testScene1", "Hello World 2!!!",Point(480,480));
		director:replaceScene(s)	
		HS:setTimer(function() director:popScene() end, 5000)
	end, 5000)	
end
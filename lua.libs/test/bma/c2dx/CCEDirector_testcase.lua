require("bma.lang.ext.Dump")

include("bma.c2dx.CCEDirector")

if true then
	local cls = bma.c2dx.CCEDirector;	
	local HS = class.instance("bma.host.Service")
	
	print("Director simple test")
	cls.pushScene("test1","Hello World!!!",240,240)	
	
	HS:setTimer(function()
		cls.replaceScene("test1","Hello World 2!!!",480,480)	
		HS:setTimer(function() cls.popScene() end, 5000)
	end, 5000)	
end
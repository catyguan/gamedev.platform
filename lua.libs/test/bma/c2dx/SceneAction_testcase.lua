require("bma.lang.ext.Dump")

include("bma.c2dx.CCEDirector")
local cls = bma.c2dx.CCEDirector;	
local acls = bma.c2dx.CCEActor;

if false then
	cls.sendCommand("char1","statusSwitch","walk","walkBack")
end

if false then
	local a1 = acls.new("char1")
	var_dump(a1)
	a1:statusSwitch("walk","walkBack")
end

if true then
	local a1 = acls.new("char1")	
	
	local cf = function(f, x,y)
		return function(err)
			print("move by",x,y)
			a1:aicall(f, "walk", x, y, 3.0)		
		end
	end

	cf(cf(cf(cf(function()end,0,180),180,0),0,-180),-180,0)()
end
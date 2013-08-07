require("bma.lang.ext.Dump")
require("bma.c2dx.Common")

local TC = 1

if TC==1 then
	print("DIY", TC)
	
	local obj1 = application:createObject("CCLayerColor", {r=255}, 740, 150)
	obj1:position({x=200,y=200})
	obj1:opacity(128)
	
	local label = application:createObject("CCLabelTTF", "Hello World\nIt's my first day to come in\nLet's go", "Arial", 64)
	label:alignLeft()
	
	local obj2 = application:createObject("CCENarrate", label, 740, 150,0, 1)
	obj2:pagePauseTime(0)
	obj2:position({x=200,y=200})	
	
	local s = application.director:scene()
	s:addChild(obj1)
	s:addChild(obj2)
	-- print("create = ",s)
	-- director:pushScene(s)	
	
	obj2:process(function(isEnd)
		print("hello", isEnd)
	end)
end
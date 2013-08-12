require("bma.lang.ext.Dump")
require("bma.c2dx.Common")

local TC = -1

if TC==-1 then
	print("DIY", TC)
	local cfg = {
		_type="CCLayerColor",
		color={r=255},
		x=200,y=200,width=740,height=150,
		opacity=128,
		children={
			_type="CCLabelTTF",
			content="Hello world",
			fontName="Arial",
			fontSize=36,
		},
	}
	local obj1 = director:buildObject(cfg)
	
	local s = director:scene()
	s:addChild(obj1)
end

if TC==1 then
	print("DIY", TC)
	
	local obj1 = application:createObject("CCLayerColor", {r=255}, 740, 150)
	obj1:position({x=200,y=200})
	obj1:opacity(128)
	
	-- local str = "Hello World\nIt's my first day to come in\nLet's go"
	local str = "诗，言其志也；\n歌，咏其声也；\n舞，动其容也；\n三者本于心，然后乐器从之。"
	str = str .. "\n凤额绣帘高卷，兽镮朱户频摇。\n两竿红日上花梢，春睡恹恹难觉。\n好梦枉随飞絮，闲愁浓胜香醪。\n不成雨暮与云朝，又是韶光过了。"
	local label = application:createObject("CCLabelTTF", str, "Arial", 36)
	label:alignLeft()
	
	local obj2 = application:createObject("CCENarrate", label, 740, 150, 0.75)
	obj2:position({x=200,y=200})	
	
	local s = application.director:scene()
	s:addChild(obj1)
	s:addChild(obj2)
	-- print("create = ",s)
	-- director:pushScene(s)	
	
	local cb
	cb = function(isEnd)		
		print("callback", isEnd)
		if not isEnd then
			print("continue")
			obj2:process(cb)
		end
	end	
	obj2:process(cb)
end


if TC==2 then
	print("DIY", TC)
	
	local sz = application.director:winSize()
	print("winSize", sz.width, sz.height)
		
	local touch = application:createObject("CCELayerTouch")
	
	local str = "Hello World\nIt's my first day to come in\nLet's go"	
	local obj2 = application:createObject("CCEDialogue")	
	obj2:position({x=0,y=200})
	touch:addChild(obj2)
	
	local s = application.director:scene()
	-- s:addChild(obj1)
	s:addChild(touch)
	-- s:addChild(obj2)
	
	obj2:show({
		size = {width=sz.width, height=170},
		fontSize = 64,
		frameImage = "dialog.png",
		margin = {left=64, right=64},
	})
	
	obj2:show({text=str}, function()
		print("show 1 end")
		local str = "诗，言其志也；\n歌，咏其声也；\n舞，动其容也；\n三者本于心，然后乐器从之。"
		obj2:show({text=str}, function()
			print("show 2 end")
			obj2:visible(false)
		end)
	end)
end
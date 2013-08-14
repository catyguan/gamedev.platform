require("bma.lang.ext.Dump")
require("bma.c2dx.Common")

local TC = 2

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
	
	local cfg = {
		_type="CCLayerColor",
		color={r=255},
		x=200,y=200,width=740,height=150,
		opacity=128,
	}
	local obj1 = director:buildObject(cfg)
	
	-- local str = "Hello World\nIt's my first day to come in\nLet's go"
	local str = "诗，言其志也；\n歌，咏其声也；\n舞，动其容也；\n三者本于心，然后乐器从之。"
	str = str .. "\n凤额绣帘高卷，兽镮朱户频摇。\n两竿红日上花梢，春睡恹恹难觉。\n好梦枉随飞絮，闲愁浓胜香醪。\n不成雨暮与云朝，又是韶光过了。"
	local cfg = {
		_type="CCENarrate",
		x=200,y=200,width=740,height=150,
		speed=0.75,
		label = {
			_type="CCLabelTTF",
			content=str,
			fontName="Arial",
			fontSize=36,
			horizontalAlignment=0
		}
	}
	local obj2 = director:buildObject(cfg)
	-- sobj2:build()
	
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
	
	local sz = director:winSize()
	print("winSize", sz.width, sz.height)
		
	local touch = director:createObject("CCELayerTouch")
	
	-- frameImage = "dialog.png",
	local cfg = {
		_type="CCEDialogue",
		x=0,y=200,width=sz.width,height=170,
		margin = {left=64, top=10, right=64, bottom=10},
		frame = {
			_type="CCScale9Sprite",
			image="dialog.png",
		},
		label = {
			_type="CCLabelTTF",
			fontName="Arial",
			fontSize=64,
			horizontalAlignment=0
		},
		pauseNode = {
			_type="CCSprite",
			image="hand1.png",
			x=sz.width-10-27,y=30,
			methods = {
				startAnimation = function(obj)
					print("startAnimation", obj)
					local cfg = {
						"forever",
						{
							"seq",
							{"moveBy", 0.4, {0, 20}},
							{"moveBy", 0.4, {0, -20}}
						}
					}
					local act = director:buildAction(cfg)
					obj:y(30)
					obj:runAction(act)
				end
			}
		}
	}
	local obj2 = director:buildObject(cfg)	
	touch:addChild(obj2)
	
	local s = director:scene()
	-- s:addChild(obj1)
	s:addChild(touch)
	-- s:addChild(obj2)

	local str = "Hello World\nIt's my first day to come in\nLet's go"		
	obj2:show({text=str}, function()
		print("show 1 end")
		local str = "诗，言其志也；\n歌，咏其声也；\n舞，动其容也；\n三者本于心，然后乐器从之。"
		obj2:show({text=str}, function()
			print("show 2 end")
			obj2:visible(false)
		end)
	end)
end
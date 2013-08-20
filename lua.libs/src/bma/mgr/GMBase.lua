-- bma/mgr/GMBase.lua
local Class = class.define("bma.mgr.GMBase",{})

function Class:getClassName(clsName, submod)
	if clsName:sub(1,1)=="@" then
		return clsName:sub(2)
	end
	if self.packageName~=nil then
		local sp
		if submod~=nil then
			sp = "." .. submod .. "."
		else
			sp = "."
		end
		return self.packageName..sp..clsName
	end
	return clsName
end

function Class:runScene(clsName, ...)
	local cname = self:getClassName(clsName, "scenes")
	local obj = class.new(cname)
	obj:init(...)
	local scene = obj:create()
	if scene==nil then
		if LOG:debugEnabled() then
			LOG:debug("GMBase",cname.." create scene fail")
		end
		return
	end
	if LOG:debugEnabled() then
		LOG:debug("GMBase","runScene "..cname)
	end
	
	if CONFIG.DEV then
		local devButton = director:buildObject({
			_type="CCEButton",
			node = {
				_type="CCSprite",
				id="DEV_BUTTON",
				image="dev.png"
			},
			touch = "focus, tap"
		})
		local winSize = director:winSize()
		local sz = devButton:contentSize()
		devButton:position({x=winSize.width-sz.width/2,y=winSize.height-sz.height/2})
		scene:addChild(devButton,100)
		
		devButton:onEvent("tap", "devTap", function()
			GM:onDevButtonClick()
		end)
	end
	
	director:runScene(scene)
end

function Class:unloadScene(clsName)
	local cname = self:getClassName(clsName, "scenes")
	class.unset(cname)
	unrequire(cname)
end

function Class:deployService(clsName)	
	s = class.new(self:getClassName(clsName, "services"))
	
	local app = class.instance("application")
	if app.services[s.id]==nil then
		s:install()		
	end
	return s
end

function Class:nextFrame(f)
	local s = director:scene()
	if s~=nil then
		s:nextFrame(f)
	else
		f()
	end
end
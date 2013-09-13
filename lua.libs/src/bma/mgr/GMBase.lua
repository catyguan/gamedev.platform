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

function Class:beforeRunScene(f)
	if not self._beforeRunScene then
		self._beforeRunScene = {}
	end
	table.insert(self._beforeRunScene, {f=f})
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
	
	if self._beforeRunScene then
		for _, o in ipairs(self._beforeRunScene) do
			o.f(scene)
		end
	end
	
	director:runScene(scene)
end

function Class:unloadScene(clsName)
	local cname = self:getClassName(clsName, "scenes")
	class.unset(cname)
	unrequire(cname)
end

function Class:deployService(sid, clsName)	
	local app = class.instance("application")
	if app.services[sid]==nil then
		local name = self:getClassName(clsName, "services")
		local s = class.new(name)
		if s.id ~= sid then error(name.." serviceId not match - "..sid) end
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

function Class:createDevButtonFunction()	
	return function(scene)
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
		-- devButton:position({x=winSize.width-sz.width/2,y=winSize.height-sz.height/2})
		devButton:position({winSize.width-sz.width/2, winSize.height-sz.height/2})
		scene:addChild(devButton,100)
		
		devButton:onEvent("tap", "devTap", function()
			GM:onDevButtonClick()
		end)
	end
end

function Class:createHttpClientStatusFunction()
	local s =  class.instance("bma.http.client.Service")
	if not s then return function() end end
	
	local fobj = {}
	fobj.doShowHide = function(self, sp, show)
		if show then
			local cfg = {
				"forever",
				{"rotateBy", 0.5, 360},
			}
			local action = director:buildAction(cfg)			
			sp:visible(true)
			sp:runAction(action)
		else
			sp:stopAllActions()
			sp:visible(false)			
		end
	end
	fobj.onRunScene = function(self, scene)
		local sp = director:buildObject({
			id="icon_httpclient",
			_type="CCSprite",
			image="httpclient.png",
			attributes = { nopause = true }
		})
		local winSize = director:winSize()
		local sz = sp:contentSize()
		sp:position({x=sz.width/2,y=winSize.height-sz.height/2})
		sp:visible(false)
		scene:addChild(sp,100)
		if s:isRunning() then
			self:doShowHide(sp, true)
		end		
	end
	fobj.showHide = function(self, show)
		local scene = director:scene()
		if scene then
			local sp = scene.icon_httpclient
			if sp then
				self:doShowHide(sp, show)
			end
		end
	end
	
	s:addListener(function(event)
		if event.type=="running" then
			fobj:showHide(event.status)
		end
	end)
	return function(scene)
		fobj:onRunScene(scene)
	end
end

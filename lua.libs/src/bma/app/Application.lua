-- bma/app/Application.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")

local Class = class.define("bma.app.Application",{})

function Class.instance()
	return class.instance("application")
end

function Class:install()
	class.setInstance("application", self)
end

function Class.startup()
	local appClassName = "app."..LUA_APP_TYPE..".Application"
	local app = class.new(appClassName)
	app:install()
	if not app:init() then
		error("application["..appClassName.."] init fail")
	end
end

function Class.shutdown(destroy)
	local app = Class.instance()
	if app then
		return app:close(destroy)
	end
	return true
end

function Class:ctor()
	self.scenes = {}
end

function Class:init()
	self:onLaunch()
	return true
end

function Class:onLaunch()
	self.launched = true
	if self.waiting ~= nil then
		for _,c in ipairs(self.waiting) do
			pcall(c)
		end
	end
	self.waiting = nil
end

function Class:close()
	
end

function Class:status()
	local r = {}
	r.memuse = os.memuse()
	return r
end

function Class:appCommand(callId, sceneName, commandName, ...)

	if not self.launched then
		if not self.waiting then self.waiting = {} end
		if LOG:debugEnabled() then
			LOG:debug("Application","waiting for launch")
		end
		local a = {...}
		table.insert(self.waiting, function()
			self:appCommand(callId,sceneName, commandName, unpack(a))
		end)
		return true
	end 
	
	local cb = function(err, ...)
		local HS = class.instance("bma.host.Service")
		HS:aiResponse(callId, err, ...)
	end
	self:sceneCommand(cb, sceneName, commandName, ...)	
	return true
end

function Class:sceneCommand(cb, sceneName, commandName, ...)
	local s = self.scenes[sceneName]
	if s==nil then
		aicall.done(cb, "scene["..sceneName.."] invalid")
		return true
	end
	return s:sceneCommandCall(cb, commandName, ...)	
end

function appCommand(callId, sceneName, commandName,...)
	local app = Class.instance()
	if app then
		return app:appCommand(callId, sceneName, commandName, ...)
	end
end

appShutdown = Class.shutdown
appStatus = function()
	local app = Class.instance()
	if app and app.status then
		return app:status()
	end
	return {}
end
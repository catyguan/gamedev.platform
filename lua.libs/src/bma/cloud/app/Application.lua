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
	self.cases = {}
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

function Class:appCommand(callId, caseName, methodName, session, ...)

	if not self.launched then
		if not self.waiting then self.waiting = {} end
		if LOG:debugEnabled() then
			LOG:debug("Application","waiting for launch")
		end
		local a = {...}
		table.insert(self.waiting, function()
			self:appCommand(callId, caseName, methodName, session, unpack(a))
		end)
		return true
	end 
	
	local cb = function(err, ...)
		local HS = class.instance("bma.host.Service")
		HS:aiResponse(callId, err, ...)
	end
	self:caseMethodCall(cb, caseName, methodName, session, ...)	
	return true
end

function Class:caseMethodCall(cb, caseName, methodName, session, ...)
	local s = self.cases[caseName]
	if s==nil then
		aicall.done(cb, "Case["..caseName.."] invalid")
		return true
	end
	return s:caseMethodCall(cb, methodName, session, ...)	
end

function appCommand(callId, caseName, methodName,session, ...)
	local app = Class.instance()
	if app then
		return app:appCommand(callId, caseName, methodName, session, ...)
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
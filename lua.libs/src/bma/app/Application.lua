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

function Class.shutdown()
	local app = Class.instance()
	if app then
		app:close()
	end
end

function Class:ctor()
	self.services = {}
end

function Class:getDesc()
	return {}
end

function Class:getService(name)
	local s = self.services[name]
	if s~=nil then return s end
	s = self:createService(name)
	if s~=nil then
		self.services[name] = s
		return s 
	end	
	local fac = self["service"..name:caps()]
	if fac~=nil and type(fac)=="function" then
		s = fac(name)
		if s~=nil then
			self.services[name] = s
			return s
		end
	end
	return nil
end

function Class:createService(name)	
	return nil
end

function Class:appCall(serviceName,apiName,...)
	local s = self:getService(serviceName)
	if s==nil then
		error("invalid service ["..serviceName.."]")
	end
	local api = s:getApi(apiName)
	local t = type(api)
	if t=="function" then
		return api(...)
	end
	if t=="table" then
		return api:execute(...) 
	end
	error("invalid api ["..serviceName..":"..apiName.."]")	
end
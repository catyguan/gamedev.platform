-- bma/app/ApplicationBase.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")

local Class = class.define("bma.app.ApplicationBase",{})

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

function Class:ctor()
	self.services = {}
end

function Class:init()	
	return true
end

function Class:close()
	
end

function SERVICE(id)
	local app = class.instance("application")	
	return app.services[id]
end
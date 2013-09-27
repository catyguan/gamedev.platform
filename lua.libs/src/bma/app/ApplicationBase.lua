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
	
end

function Class:init()	
	return true
end

function Class:close()
	
end

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

APP_CLASS_NAME = function(clsName, submod)
	local app = class.instance("application")
	return app:getClassName(clsName, submod)
end

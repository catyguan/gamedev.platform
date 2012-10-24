-- bma/app/Application.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")

local Class = class.define("bma.app.AppService",{})

function Class:ctor()
	self.apis = {}
end

function Class:getDesc()
	return {}
end

function Class:getApi(name)
	local s = self.apis[name]
	if s~=nil then return s end
	s = self:createApi(name)
	if s~=nil then
		self.apis[name] = s
		return s 
	end
	s = self["api"..name:caps()]
	if s~=nil and type(s)=="function" then
		self.apis[name] = s
		return s
	end
	return nil
end

function Class:createApi(name)
	return nil
end
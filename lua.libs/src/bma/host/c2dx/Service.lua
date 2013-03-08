-- bma/host/c2dx/Service.lua
require("bma.lang.ext.Core")
require("bma.host.CommonLuaHostService")

local super = bma.host.CommonLuaHostService
local Class = class.define("bma.host.c2dx.Service",{super})
local LTAG = "c2dxHost"
local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.C2dxHost or EMPTY_TABLE

function Class.install()
	if not instance then instance = Class.new() end		
	class.setInstance("bma.host.Service", Class.new())		
end

function Class:ctor()
	_G.director = luahost.call("application")
end

function Class:aiResponse(callId, ...)
	local fn = function(r1,r2)
		LOG:debug("host.BaseService", "aiResponse => "..tostring(r1).."/"..tostring(r2))
	end
	self:call(fn, "airesponse", callId, ...)
end

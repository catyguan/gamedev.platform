-- bma/host/dotnet/Service.lua
require("bma.lang.ext.Core")
require("bma.host.CommonLuaHostService")

local super = bma.host.CommonLuaHostService
local Class = class.define("bma.host.dotnet.Service",{super})
local LTAG = "dotnetHost"
local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.DotnetHost or EMPTY_TABLE

function Class.install()
	if not instance then instance = Class.new() end		
	class.setInstance("bma.host.Service", Class.new())		
end

function Class:ctor()
	_G.director = luahost.call("application")
end

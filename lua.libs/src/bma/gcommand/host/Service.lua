-- bma/gcommand/host/Service.lua
require("bma.lang.ext.Core")
require("bma.gcommand.BaseService")

local Class = class.define("bma.gcommand.host.Service", {bma.gcommand.BaseService})

function Class.install()
	local o = Class.new()
	class.setInstance("bma.gcommand.Service", o)
end

function Class:ctor()
	
end

function Class:processGlobalCommandCall(cb, accessId, sceneName, commandName, ...)
	local HS = class.instance("bma.host.Service")
    return HS:call(cb, "gcommand.execute", accessId, sceneName, commandName, {...})
end
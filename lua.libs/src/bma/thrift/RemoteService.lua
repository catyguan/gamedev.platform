-- bma/thrift/RemoteService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.thrift.RemoteService",{})
local LTAG = "TRemote"
local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.ThriftRemote or EMPTY_TABLE

local SDK = director.thriftRemote

function Class.install()	
	class.setInstance("bma.thrift.RemoteService", Class.new())			
end

function Class:ctor()
	self.initGate = false
end

function Class:isOpen()	
	return SDK:isConnect()
end

function Class:gates()
	
end

function Class:open()
	if not self.initGate then
		SDK:setServiceAPI("_API_ThrfitRemote_service")
		SDK:clearGate()
		SDK:addGate("172.19.16.195", 9093)
	end
	self.initGate = true
	SDK:connect()
end

function Class:call(name, msg, timeout, callResult, callError, callTimeout)
	return SDK:call(name, msg, timeout, callResult, callError, callTimeout)
end

-- API
function _API_ThrfitRemote_service(...)	
	print("ServiceAPI", ...)
end


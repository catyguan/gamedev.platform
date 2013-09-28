-- bma/http/client/host/Service.lua
require("bma.lang.ext.Core")
require("bma.http.client.BaseService")

local Class = class.define("bma.http.client.host.Service", {bma.http.client.BaseService})

function Class.install()
	local o = Class.new()
	class.setInstance("bma.http.client.Service", o)
end

local httpclient = hostapp.httpclient

function Class:process(req, callback)
	local this = self
	local r = httpclient:process(req, function(...)
		this:sendEvent({type="running", status=this:isRunning()})
		if callback then
			callback(...)
		end
	end)
	self:sendEvent({type="running", status=self:isRunning()})
	return r
end

function Class:cancel(reqid)
	local r = httpclient:cancel(reqid)
	self:sendEvent({type="running", status=self:isRunning()})
	return r
end

function Class:isRunning()
	local rc = httpclient:runningCount()
	return rc > 0	
end

function Class:escape(v)
	return httpclient:escape(v)
end
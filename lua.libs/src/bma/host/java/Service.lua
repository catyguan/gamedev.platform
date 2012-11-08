-- bma/host/java/Service.lua
require("bma.lang.ext.Core")
require("bma.host.BaseService")

local Class = class.define("bma.host.java.Service",{bma.host.BaseService})
local instance;

function Class.install()
	if not instance then instance = Class.new() end	
	class.setInstance("bma.host.Service", instance)
end

function Class:ctor()
	self.calls = {}
	self.timers = {}
end

-- return callId:int
function Class:call(opts, ...)
	local args = {...}
	local cid = self:nextCallId()
	local done,r,et = pcall(function()
    	return {java.call("hostCall", cid, unpack(args))}
	end)	
	if done then
		local syn = unpack(r,1,1)
		if syn then			
			table.remove(r,1)
			if LOG:debugEnabled() then
				LOG:debug("JavaHost","hostCall done -- T"..#r)
			end
			self:response(opts, nil, r)			
			return -1
		else									
			self.calls[cid] = opts
			return cid			
		end
	else
		if LOG:debugEnabled() then
			LOG:debug("JavaHost",tostring(r).."\n"..et)
		end
		self:response(opts, r or "<empty error message>", nil)
		return -1
	end
end

function luaCallResponse(callId, err, ...)
	if LOG:debugEnabled() then
		LOG:debug("JavaHost","luaCallResponse - callId="..callId..",err="..tostring(err))
		if not err then
			LOG:debug("JavaHost",{...})
		end
	end
	local opts = instance.calls[callId]
	if opts then
		if err then
			instance:response(opts,err)			
		else
			instance:response(opts,nil,{...})
		end
	else
		LOG:debug("JavaHost","discard response "..callId)
	end
end

-- return callId:int or boolean
function Class:tryCall(opts, ...)
	return self:call(opts, ...)
end
	
-- return boolean
function Class:canCall()
	return true
end

-- boolean	
function Class:discardCallResponse(callId)
	if callId <= 0 then return true end
	self.calls[callId] = nil
	return true
end
	
-- fun:timerFunction 定时调用的哦函数
-- delayTime:int 延迟调用的时间，毫秒
-- return timerId:int 定时器的编号
function Class:setTimer(fun, delayTime)
	local tid = self:nextTimerId()
	self.timers[tid] = fun
	java.call("hostTimer", tid, delayTime)
	return tid
end

function luaTimerResponse(timerId)	
	local fun = instance.timers[timerId]
	if fun then
		LOG:debug("JavaHost","timer "..timerId.." active")
		fun()
		return true;
	else
		LOG:debug("JavaHost","timer "..timerId.." discard")
		return false
	end
end
		
-- timerId:int
function Class:removeTimer(timerId)
	if timerId <= 0 then return true end
	self.timers[timerId] = nil
	return true
end
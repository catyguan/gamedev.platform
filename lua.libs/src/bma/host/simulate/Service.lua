-- bma/host/simulate/Service.lua
require("bma.lang.ext.Core")
require("bma.host.BaseService")

local Class = class.define("bma.host.simulate.Service",{bma.host.BaseService})

function Class.install()
	local o = Class.new()
	class.setInstance("bma.host.Service", o)
end

function Class:ctor()
	self.calls = {}
	self.timers = {}
end

-- return callId:int
function Class:call(opts, ...)
	local args = {...}
	local cid = self:nextCallId()
	table.insert(self.calls,{
		args	= args,
		opts 	= opts,
		callId 	= cid,
	})	
	return cid
end

function Class:processCall(p)
	if #self.calls > 0 then
		local c = table.remove(self.calls, 1)
		local done,r,et = pcall(function()
    		return {p(unpack(c.args))}
		end)
		if done then
			self:response(c.opts, r, nil)
		else
			LOG:debug("SimulateHost",r.et)
			self:response(c.opts,nil,r)
		end
		return true
	else
		return false
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
	if callId < 0 then return true end
	for i,v in ipairs(self.calls) do
		if v.callId==callId then
			table.remove(self.calls, i)
			return
		end
	end	
	return false
end

function Class:processTimer(t)
	local t0 = os.mclock()
	while true do
		if #self.timers > 0 then
			local v = self.timers[1]
			local t1 = os.mclock()
			if t1>=v.time then
				table.remove(self.timers,1)
				v.fun()			
			else
				if t>=0 and t0+t<t1 then break end
			end
		else
			break
		end
	end
end
	
-- fun:timerFunction 定时调用的哦函数
-- delayTime:int 延迟调用的时间，毫秒
-- return timerId:int 定时器的编号
function Class:setTimer(fun, delayTime)
	local mid = self:nextTimerId()
	table.insert(self.timers, {
		timerId = mid,
		time = os.mclock()+delayTime,
		fun = fun,
	})
	if #self.timers > 1 then
		table.sort(self.timers, function(t1,t2) return t1.time<=t2.time end)
	end
	return mid
end

-- timerId:int
function Class:removeTimer(timerId)
	if timerId < 0 then return true end
	for i,v in ipairs(self.timers) do
		if v.timerId==timerId then
			table.remove(self.timers, i)
			return
		end
	end	
	return false
end

function HSP()
	local o = class.instance("bma.host.Service")
	local r = false
	r = o:processCall(function(name,...) return _G[name](...) end)
	return o:processTimer(-1) or r
end
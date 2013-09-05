-- bma/host/CommonLuaHostService.lua
require("bma.lang.ext.Core")
require("bma.host.BaseService")

local super = bma.host.BaseService
local Class = class.define("bma.host.CommonLuaHostService",{super})
local LTAG = "luaHost"
local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.LuaHost or EMPTY_TABLE

function Class.install()	
	class.setInstance("bma.host.Service", Class.new())		
end

function Class:ctor()
	self.calls = {}
	self.closures = {}
	self.timers = {}
end

function Class:nextCallId()
	local cid = super.nextCallId(self)
	while self.calls[cid] do
		cid = super.nextCallId(self)
	end
	return cid
end

function Class:nextClosureId()
	local cid = super.nextClosureId(self)
	while self.closures[cid] do
		cid = super.nextClosureId(self)
	end
	return cid
end

function Class:nextTimerId()
	local tid = super.nextTimerId(self)
	while self.timers[tid] do
		tid = super.nextTimerId(self)
	end
	return tid
end

function Class:response2host(...)
	return luahost.call("response", ...)
end

function Class:invoke(...)
	return luahost.call("invoke", ...)
end

-- return callId:int
function Class:ainvoke(opts, ...)
	local args = {...}
	local cid = self:nextCallId()
	self.calls[cid] = opts
	local done,r,et = pcall(function()
    	return {luahost.call("ainvoke", cid, unpack(args))}
	end)	
	if done then		
		return cid		
	else
		if LDEBUG then
			LOG:debug(LTAG,tostring(r).."\n"..et)
		end
		self.calls[cid] = nil
		return self:response(opts, r or "<empty error message>", nil)
	end	
end

-- boolean	
function Class:discardAInvokeResponse(callId)
	if callId <= 0 then return true end
	self.calls[callId] = nil
	return true
end

function Class:wrapClosure(f)
	return luahost.closure(f)
end

function Class:removeClosure(cid)
	self.closures[cid] = nil
end

-- API
function _API_host_response(callId, err, ...)
	if LDEBUG then
		LOG:debug(LTAG,"_host_response - callId="..callId..",err="..tostring(err))
		if not err then
			LOG:debug(LTAG,{...})
		end
	end
	local HS = class.instance("bma.host.Service")
	local opts = HS.calls[callId]
	HS.calls[callId] = nil
	if opts then
		if err then
			HS:response(opts,err)			
		else
			HS:response(opts,nil,{...})
		end
	else
		if LDEBUG then
			LOG:debug(LTAG,"discard host response "..callId)
		end
	end
end

-- API
function _API_host_closure_call(callId, ...)
	if LDEBUG and CFG.LogClosure then
		LOG:debug(LTAG,"_host_closure_call - callId="..callId)
		print("call params => ", ...)
	end
	local HS = class.instance("bma.host.Service")
	local f = HS.closures[callId]
	if f then
		return f(...)
	else
		if LDEBUG and CFG.LogClosure then
			LOG:debug(LTAG,"discard _host_closure_call "..callId)
		end
	end
end

-- API
function _API_host_closure_add(f)	
	local HS = class.instance("bma.host.Service")
	local cid = HS:nextClosureId()
	if LDEBUG and CFG.LogClosure then
		LOG:debug(LTAG,"_host_closure_add - callId="..cid..","..tostring(f))
	end
	HS.closures[cid] = f
	return cid
end

-- API
function _API_host_closure_remove(callId)
	if LDEBUG and CFG.LogClosure then
		LOG:debug(LTAG,"_host_closure_remove - callId="..callId)
	end
	local HS = class.instance("bma.host.Service")
	HS:removeClosure(callId)
end

-- fun:timerFunction 定时调用的哦函数
-- delayTime:int 延迟调用的时间，毫秒
-- return timerId:int 定时器的编号
function Class:setTimer(fun, delayTime)
	local tid = self:nextTimerId()
	self.timers[tid] = {fun}
	luahost.call("timer", tid, delayTime, nil)
	return tid
end

-- fun:timerFunction 定时调用的哦函数
-- fixTime:int 固定执行间隔的时间，毫秒
-- delayTime:int 延迟调用的时间，毫秒
-- return timerId:int 定时器的编号
function Class:setFixTimer(fun, fixTime, delayTime)
	if not delayTime then delayTime = fixTime end 
	if delayTime<=0 then delayTime = fixTime end
	local tid = self:nextTimerId()
	self.timers[tid] = {fun,fixTime}
	luahost.call("timer", tid, delayTime, fixTime)
	return tid
end

-- timerId:int
function Class:removeTimer(timerId)
	if timerId <= 0 then return true end
	self.timers[timerId] = nil
	return true
end

-- API
function _API_host_timer_response(timerId)	
	local HS = class.instance("bma.host.Service")
	local timeInfo = HS.timers[timerId]
	local fun
	local fix
	if timeInfo then
		fun = timeInfo[1]
		fix = timeInfo[2]
	end
	if fun then
		if LDEBUG and CFG.LogTimer then
			LOG:debug(LTAG,"timer "..timerId.." active")
		end
		if not fix then HS.timers[timerId] = nil end
		local done, r1, r2 = pcall(function()
			return fun()
		end)
		if not done then
			HS.timers[timerId] = nil
			if LDEBUG then
				LOG:debug(LTAG,"timer "..timerId.." error - %s\n%s", tostring(r1), tostring(r2))
			end
			return false
		end
		if fix then
			if not r1 then
				HS.timers[timerId] = nil
			end			
			return r1
		else
			HS.timers[timerId] = nil
			return false
		end
	else
		if LDEBUG and CFG.LogTimer then
			LOG:debug(LTAG,"timer "..timerId.." discard")
		end
		return false
	end
end


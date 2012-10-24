-- bma/host/c2dx/BGTask.lua

module(..., package.seeall)

---- init & bind bgthread
queue = {}

local scheduler = CCDirector:sharedDirector():getScheduler()
local function bgthreadPeek(dt)
	local bgt = require("bgthread__api")
	hasRep, msgId,err, data = bgt.peek()
	if hasRep>0 then
		LOG:debug("BGTask", string.format("(%d,%d) %s",msgId,err,data))

		local req = table.remove(queue,1)
		if err==0 then
			local cjson = require("cjson")
			data = cjson.decode(data)
		end
		local remain = #queue
		if _listeners ~= nil then
			_listeners:fire("end", msgId, err, data)
		end
		local callDone,callErr,callTrace = pcall(req.f,msgId,err,data)
		if not callDone then
			LOG:warn("BGTask", callErr.."\n"..callTrace)
		end
		if remain > 0  then
			local nreq = queue[1]
			bgt.call(nreq.i, nreq.d)
			if _listeners ~= nil then
				_listeners:fire("begin", nreq.i, 0, nreq.d)
			end
		end

	end
end
local timer = scheduler:scheduleScriptFunc(bgthreadPeek, 0.1, false)

-- api
function call(msgId, data, fun)
	local cjson = require("cjson")
	data = cjson.encode(data)
	table.insert(queue,{i=msgId,d=data,f=fun})
	if #queue == 1 then
		local bgt = require("bgthread__api")
		bgt.call(msgId, data)
		if _listeners ~= nil then
			_listeners:fire("begin", msgId, 0, data)
		end
	end
	return true
end

function tryCall(msgId, data, fun)
	if #queue>0 then
		return false
	end
	call(msgId,data,fun)
	return true
end

function canCall()
	return #queue==0
end

function size()
	return #queue
end

-- listener
function listeners()
	if _listeners == nil then
		local Event = require("bma.lang.Event")
		_listeners = Event.ListenerHub.new()
	end
	return _listeners
end

LOG:debug("BGTask","loaded")
-- bma/host/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.host.BaseService")

function Class:ctor()
	self.callId = 1
	self.timerId = 1
end

function Class:nextCallId()
	local r = self.callId
	if self.callId >= 10000000 then 
		self.callId = 1
	else
		self.callId = self.callId + 1 
	end
	return r
end

function Class:nextTimerId()
	local r = self.timerId
	if self.timerId >= 10000000 then 
		self.timerId = 1
	else
		self.timerId = self.timerId + 1 
	end
	return r
end

function Class:callStack(opts)
	if type(opts)=="function" then
		return { handler = opts }
	else
		return opts
	end
end

function Class:response(opts,result,error)
	if error then		
		if type(opts)=="function" then 
			opts(error)
			return
		end
		if type(opts)=="table" then
			if opts['handler'] then
				opts['handler'](error)
			else if opts['error'] then
					opts['error'](error)
					return
				end
			end			
			LOG:debug("host.BaseService","miss error handler")
		end
	else
		if result==nil then result = {} end				
		if type(opts)=="function" then 
			opts(nil, unpack(result))
			return
		end
		if type(opts)=="table" then
			if opts['filter'] then
				result = opts['filter'](result);
			end
			if opts['handler'] then
				opts['handler'](nil, unpack(result))
			else if opts['success'] then
					opts['success'](unpack(result))
					return
				end
			end
			LOG:debug("host.BaseService","miss success handler")
		end
	end
	LOG:warn("host.BaseService","invalid options type"..type(opts))
end

function Class:aiResponse(callId, ...)
	local fn = function(r1,r2)
		LOG:debug("host.BaseService", "aiResponse => "..tostring(r1).."/"..tostring(r2))
	end
	self:call(fn, "airesponse", callId, ...)
end
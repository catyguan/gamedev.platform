-- bma/host/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.host.BaseService")

function Class:ctor()
	self.callId = 1
	self.closureId = 1;
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

function Class:nextClosureId()
	local r = self.closureId
	if self.closureId >= 10000000 then 
		self.closureId = 1
	else
		self.closureId = self.closureId + 1 
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

function Class:response(opts,err,result)	
	local f = function()
		if err then		
			if type(opts)=="function" then 
				opts(err)
				return
			end
			if type(opts)=="table" then
				if opts['handler'] then
					opts['handler'](err)
					return
				else if opts['error'] then
						opts['error'](err)
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
					return
				else if opts['success'] then
						opts['success'](unpack(result))
						return
					end
				end
				LOG:debug("host.BaseService","miss success handler")
			end
		end
		LOG:warn("host.BaseService","invalid options type "..type(opts))
	end
	tryCatch(f, "Host.Response")
end


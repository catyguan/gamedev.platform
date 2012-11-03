-- bma/host/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.host.AICall")

function Class:ctor(...)
	self.params = {...}
end

function Class:error(fn)	
	self.errorFunction = fn
	if self.errorData then
		fn(self.errorData)
	end
	if self.nextCall and not self.nextCall.errorFunction then
		self.nextCall:error(fn)
	end
	return self
end

function Class:next(fn)
	if type(fn)=="function" then
		self.nextFunction = fn
		if self.resultData then
			fn(unpack(self.resultData))
		end
		return self
	end
	if type(fn)=="table" and fn.className==Class.className then
		self.nextCall = fn
		if not fn.errorFunction and self.errorFunction then
			fn:error(self.errorFunction)
		end 	
		return self:next(function(...)
			fn:run(...)
		end)
	end
	error("unknow type "..type(fn))		
end

function Class:run(...)
	local s = self
	local f = function(err,...)
		if err then
			if s.errorFunction then
				s.errorFunction(err)
			else
				s.errorData = err
			end
		else
			if s.nextFunction then
				s.nextFunction(...)
			else
				s.resultData = {...}
			end
		end
	end

	local ps
	if arg and arg.n>0 then
		ps = {}
		for i,v in ipairs(self.params) do
			ps[i] = v			
		end
		for i = 1, arg.n do
            ps[#ps+1] = arg[i]
        end
    else
    	ps = self.params
    end
	
	local HS = class.instance("bma.host.Service")
	HS:call(f, unpack(ps))
	return self

	
end

function Class:response(callId)
	return self:next(function(...)
		local HS = class.instance("bma.host.Service")
		HS:aiResponse(callId,...)	
	end)	
end

function aicall(...)
	return Class.new(...)
end
-- bma/app/AppCase.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")
require("bma.app.protocol.Common")

local P = bma.app.protocol.Common;
local Class = class.define("bma.app.AppCase")

function Class:ctor()
	self.methods = {}
end

function Class:getMethod(name)
	local s = self.methods[name]
	if s~=nil then return s end
	s = self:createMethod(name)
	if s~=nil then
		self.methods[name] = s
		return s 
	end
	s = self[name.."Method"]
	if s~=nil and type(s)=="function" then
		self.methods[name] = s
		return s
	end
	return nil
end

function Class:createMethod(name)
	return nil
end

function Class:caseMethodCall(cb, methodName, ...)
	local m = self:getMethod(methodName)
	if m then		
		self:pcall(cb, function(...)
			print(methodName, ...)
			m(self, cb, ...)
		end, ...)
	else
		aicall.done(cb, "invalid method ["..methodName.."]")
	end	
	return true 
end

function Class:pcall(cb, fn, ...)
	local r1,r2,r3 = pcall(fn, ...)
	if not r1 then
		if LOG:debugEnabled() then
			LOG:debug(self.className, "%s\n%s", tostring(r2), tostring(r3))
		end
		self:error(cb, r2)
	end
end

function Class:isNull(cb, n, v)
	if v~=nil then
		return false
	end
	self:mistake(cb, "value.null", "'"..n.."' value is null")
	return true
end

function Class:response(cb, err, data)
	aicall.done(cb, err, data)
end

function Class:done(cb, err, r)
	if not err then
		r = {success=true, result=r}
	end
	aicall.done(cb, err, r)
end

function Class:success(cb, r)
	r = {success=true, result=r}
	aicall.done(cb, nil, r)
end

function Class:mistake(cb, code, msg)
	aicall.done(cb, nil, P.mistake(code, msg))
end

function Class:error(cb, code, msg)
	aicall.done(cb, nil, P.error(code, msg))
end

function Class:pass(cb, err , r)
	if err then
		aicall.done(cb, err)
		return
	end
	if r~=nil and type(r)=="table" then
		if r.mistake or r.error or r.forward then
			aicall.done(cb,nil, r)
			return			
		end
	end
	self:done(cb,nil,r)
end

function Class:forward(cb, gname, caseName, methodName, session,  ...)
	aicall.done(cb, nil, P.forward(gname, caseName, methodName, session, ...))
end


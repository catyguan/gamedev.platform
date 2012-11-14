-- bma/lang/Entity.lua
require("bma.lang.ext.Core")
require("bma.lang.ai.Core")

local EM = class.define("bma.lang.EntityManager")
local LTAG = "EntityManager"
local AIG = bma.lang.ai.Group

EM.syn = false

function EM.serializeValue(cb, v)
	if type(v) == "table" then
		if v.className~=nil then
			return EM.storeObject(cb, v)
		end
		local cb1 = function(err,g)
			cb(nil, g:toTable())
		end		
		local g = AIG.new(cb1, AIG.logError(LTAG, "serializeValue fail - %s"))
		-- print("SV-T: "..string.dump(v))
		for k,v2 in pairs(v) do
			-- print("SV-T-I: "..k.."="..string.dump(v2))
			EM.serializeValue(g:call(k), v2)
		end
		return g:finishAll()
	elseif type(v) == "function" then
		return aicall.done(cb,nil,nil)
	end	
	aicall.done(cb,nil,v)
	return true
end

function EM.serializeObject(cb,cls,obj,exclude)
	local data = {}
	if exclude == nil then exclude = {} end
	class.walk(cls,function(cls)
		if cls.SERIALIZE ~= nil then
			for k,v in pairs(cls.SERIALIZE) do
				exclude[k] = v
			end
		end
	end)
	local cb1 = function(err,g)
		cb(nil, g:toTable())
	end	
	local g = AIG.new(cb1, AIG.logError(LTAG, "serializeObject fail - %s"))
	-- print("SV-O: "..string.dump(obj))
	for k,v in pairs(obj) do
		local r,ret
		if exclude ~= nil then
			ret = exclude[k]
		end
		if V(ret,true) then			
			-- print("SV-O-I: "..k.."="..string.dump(v))	
			EM.serializeValue(g:call(k), v)
		end
	end
	return g:finishAll()
end

function EM.deserializeValue(cb, data)
	if type(data)=="table" then
		if data._class~=nil then			
			return EM.restoreObject(cb, data)
		else
			local cb1 = function(err,g)							 
				cb(nil, g:toTable())
			end	
			local g = AIG.new(cb1, AIG.logError(LTAG, "deserializeValue fail - %s"))
			-- print("SV-T: "..string.dump(data))
			for k,v in pairs(data) do
				-- print("SV-T-I: "..k.."="..string.dump(v))
				EM.deserializeValue(g:call(k), v)
			end
			return g:finishAll()
		end
	end
	aicall.done(cb, nil, data)
	return true
end

function EM.deserializeObject(cb, obj,data,exclude)
	local cb1 = function(err,g)							 
		local t = g:toTable()
		for k,v in pairs(t) do
			obj[k] = v
		end
		aicall.done(cb, nil, obj)
	end	
	local g = AIG.new(cb1, AIG.logError(LTAG, "deserializeObject fail - %s"))
	-- print("SV-O: "..string.dump(data))
	for k,v in pairs(data) do
		if k~="_class" then
			-- print("SV-O-I: "..k.."="..string.dump(v))
			EM.deserializeValue(g:call(k), v)
		end
	end
	return g:finishAll()
end

function EM.storeObject(cb, v)
	if v.className~=nil then
		local clsName = v.className
		local cb1 = function(err, data)
			if data then
				data._class = clsName 
			end
			cb(err, data)
		end
		
		if v.storeObject ~= nil then
			return v:storeObject(cb1)
		else
			local cls = class.forName(clsName)
			if cls == nil then
				error(string.format("class[%s] not exists"),clsName)
			end
			return EM.serializeObject(cb1, cls, v)
		end
	end
	aicall.done(cb,nil,nil)
	return true
end

function EM.restoreObject(cb, data)
	if data==nil then 
		aicall.done(cb,nil,nil)
		return true
	end	
	local cls = class.forName(data._class)
	if cls == nil then
		cb("class["+tostring(data._class)+"] not exists")
		return true
	end	
	if cls.entityFactory and type(cls.entityFactory)=="function" then
		return cls.entityFactory(cb, cls, data)
	else
		local o = class.new(data._class)
		if o.restoreObject ~= nil then
			return o:restoreObject(cb, data)
		else
			return EM.deserializeObject(cb, o, data)
		end
	end	
end

local Entity = class.define("bma.lang.Entity")

function Entity:isStateModify()
	if self._sM~=nil then
		return self._sM
	end
	return false
end

function Entity:stateModify(b)
	self._sM = b;
end

function Entity:hasProp(n)
	if self._p==nil then return false end
	return self._p[n] ~= nil
end

function Entity:removeProp(n)
	if self._p==nil then return end
	self._p[n] = nil
end

function Entity:prop(p1,p2)
	if p2==nil then
		if self._p == nil then return nil end
		return self._p[p1]
	else
		if self._p == nil then self._p = {} end
		if self._p[p1] ~= p2 then
			self._p[p1] = p2
			self:stateModify(true)
		end
		return p2
	end
end

function Entity:allProp()
	return self._p or EMPTY_TABLE
end

function Entity:hasRunv(n)
	if self._r ==nil then return false end
	return self._r[n] ~= nil
end

function Entity:removeRunv(n)
	if self._r ==nil then return end
	self._r[n] = nil
end

function Entity:runv(p1,p2)
	if p2==nil then
		if self._r == nil then return nil end
		return self._r[p1]
	else
		if self._r == nil then self._r = {} end
		self._r[p1] = p2
		return p2
	end
end

function Entity:allRunv()
	return self._r or EMPTY_TABLE
end

function Entity:storeObject(cb)	
	if self._p~=nil then
		return EM.serializeValue(cb, self._p)
	end
	aicall.done(cb, nil, nil)
	return true
end

function Entity:restoreObject(cb, data)
	local cb1 = function(err, state)		
		if state then
			self._p = state
		end
		self:stateModify(false)
		cb(nil, self)
	end
	return EM.deserializeValue(cb1, data)
end

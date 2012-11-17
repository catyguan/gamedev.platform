-- bma/lang/Class.lua

require("bma.lang.ext.Core")

class.getMethod = function(o,name)
	local m = o[name]
	if m~=nil and type(m)=="function" then
		return m
	end
	return nil
end

class.serializeValue = function(v)
	if type(v) == "table" then
		if v.className~=nil then
			return class.saveObject(v)
		end
		local r = {}
		for k,v2 in pairs(v) do
			r[k] = class.serializeValue(v2)
		end
		return r
	elseif type(v) == "function" then
		return nil
	end
	return v
end

class.serializeObject = function(cls,obj,exclude)
	local data = {}
	if exclude == nil then exclude = {} end
	class.walk(cls,function(cls)
		if cls.SERIALIZE ~= nil then
			for k,v in pairs(cls.SERIALIZE) do
				exclude[k] = v
			end
		end
	end)
	for k,v in pairs(obj) do
		local r,ret
		if exclude ~= nil then
			ret = exclude[k]
		end
		if V(ret,true) then
			data[k] = class.serializeValue(v)
		end
	end
	return data
end

class.deserializeValue = function(data)
	if type(data)=="table" then
		if data._class~=nil then
			return class.loadObject(data)
		else
			local r = {}
			for k,v in pairs(data) do
				r[k] = class.deserializeValue(v)
			end
			return r
		end
	end
	return data
end

class.deserializeObject = function(obj,data,exclude)
	for k,v in pairs(data) do
		if k~="_class" then
			obj[k] = class.deserializeValue(v)
		end
	end
end

class.saveObject = function(v)
	if v.className~=nil then
		local data
		local clsName = v.className
		if v.saveObject ~= nil then
			data = v:saveObject()
		else
			local cls = class.forName(clsName)
			if cls == nil then
				error(string.format("class[%s] not exists"),clsName)
			end
			data = class.serializeObject(cls, v)
		end
		if data ~= nil then
			data._class = clsName
			return data
		end
	end
	return nil
end

class.loadObject = function(data)
	local cls = class.forName(data._class)
	if cls == nil then
		error(string.format("class[%s] not exists", name))
	end	
	
	local o
	if cls.instanceFactory and type(cls.instanceFactory)=="function" then
		local fo, inited = cls.instanceFactory(cls, data)
		if fo then			
			if inited then
				return fo
			end
			o = fo
		end		
	end
	
	if not o then
		o = class.new(data._class)
	end
	if o.loadObject ~= nil then
		o:loadObject(data)
	else
		class.deserializeObject(o, data)
	end
	return o
end

local StdObject = class.define("bma.lang.StdObject")

-- sub class use
function StdObject:_access(t,n,v)
	t[n] = v
end

function StdObject:id(v)
	return self:attr("id",v)
end

function StdObject:tag(v)
	return self:attr("tag",v)
end

function StdObject:hasThiv(n)	
	return self[n]~=nil
end

function StdObject:removeThisv(n)
	self:_access(self,n,nil)
end

function StdObject:thisv(p1,p2)
	if p2==nil then
		return self[p1]
	else
		self:_access(self,p1,p2)
		return p2
	end
end

function StdObject:hasProp(n)
	if self._prop == nil then return false end
	return self._prop[n]~=nil
end

function StdObject:removeProp(n)
	if self._prop==nil then return end
	self:_access(self._prop,n,nil)
end

function StdObject:prop(p1,p2)
	if p2==nil then
		if self._prop == nil then return nil end
		return self._prop[p1]
	else
		if self._prop == nil then self._prop = {} end
		self:_access(self._prop,p1,p2)
		return p2
	end
end

function StdObject:allProp()
	if self._prop then return READONLY(self._prop) end
	return EMPTY_TABLE
end

function StdObject:hasAttr(n)
	if self._attr == nil then return false end
	return self._attr[n]~=nil
end

function StdObject:removeAttr(n)
	if self._attr == nil then return end
	self:_access(self._attr,n,nil)
end

function StdObject:attr(p1,p2)
	if p2==nil then
		if self._attr == nil then return nil end
		return self._attr[p1]
	else
		if self._attr == nil then self._attr = {} end
		self:_access(self._attr,p1,p2)
		return p2
	end
end

function StdObject:allAttr()
	if self._attr then return READONLY(self._attr) end
	return EMPTY_TABLE
end

function StdObject:hasRunv(n)
	if self._runv == nil then return false end
	return self._runv[n]~=nil
end

function StdObject:removeRunv(n)
	if self._runv==nil then return end
	self:_access(self._runv,n,nil)
end

function StdObject:runv(p1,p2)
	if p2==nil then
		if self._runv == nil then return nil end
		return self._runv[p1]
	else
		if self._runv == nil then self._runv = {} end
		self:_access(self._runv, p1, p2)
		return p2
	end
end

function StdObject:allRunv()
	if self._runv then return READONLY(self._runv) end
	return EMPTY_TABLE
end

function StdObject:hasTempv(n)
	if self._tempv == nil then return false end
	return self._tempv[n]~=nil
end

function StdObject:removeTempv(n)
	if self._tempv==nil then return end
	self:_access(self._tempv,n,nil)
end

function StdObject:tempv(p1,p2)
	if p2==nil then
		if self._tempv == nil then return nil end
		return self._tempv[p1]
	else
		if self._tempv == nil then self._tempv = {} end
		self:_access(self._tempv,p1,p2)
		return p2
	end
end

function StdObject:allTempv()
	if self._tempv then return READONLY(self._tempv) end
	return EMPTY_TABLE
end

function StdObject:saveObject()
	local r = {}
	if self._attr ~=nil then
		r.attr = class.serializeValue(self._attr)
	end
	if self._prop ~=nil then
		r.prop = class.serializeValue(self._prop)
	end	
	return r
end

function StdObject:loadObject(data)
	if data.attr~=nil then
		self._attr = class.deserializeValue(data.attr)
	end
	if data.prop~=nil then
		self._prop = class.deserializeValue(data.prop)
	end
end

if table.filter==nil then table.filter = {} end
table.filter.BY_ATTR = function(n,v)
	return function(k,o)
		if type(o)=="table" and o._attr~=nil then
			return o:attr(n)==v
		end
		return false
	end
end
table.filter.BY_PROP = function(n,v)
	return function(k,o)
		if type(o)=="table" and o._prop~=nil then
			return o:prop(n)==v
		end
		return false
	end
end
table.filter.BY_RUNV = function(n,v)
	return function(k,o)
		if type(o)=="table" and o._runv~=nil then
			return o:runv(n)==v
		end
		return false
	end
end
table.filter.BY_TEMPV = function(n,v)
	return function(k,o)
		if type(o)=="table" and o._tempv~=nil then
			return o:tempv(n)==v
		end
		return false
	end
end

-- <<class part - Singleton>>
local Singleton = class.define("bma.lang.Singleton")

function Singleton.instanceFactory(cls, data)
	return class.instance(cls.className)
end

-- <<class part - RuntimeObject>>
local RuntimeObject = class.define("bma.lang.RuntimeObject")

function RuntimeObject:saveObject()
	return EMPTY_TABLE
end

function RuntimeObject:loadObject(data)
	
end
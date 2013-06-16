-- bma/lang/Class.lua

require("bma.lang.ext.Core")

class.getMethod = function(o,name)
	local m = o[name]
	if m~=nil and type(m)=="function" then
		return m
	end
	return nil
end

class.shortClassName = function(o)
	local s = o.className
	local len = s:len()
	for i=len,1,-1 do
		if s:byte(i)==46 then
			return s:sub(i+1)
		end
	end
	return s
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

-- << class, StdObject >>
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

function StdObject:thisArray(p1)
	if self[p1]==nil then
		return self:thisv(p1, {})
	end
	return self:thisv(p1)
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

function StdObject:addProp(n,v,maxv,minv)
	local old = self:prop(n)
	local nv = old+v
	if maxv~=nil then
		if nv>maxv then nv=maxv end
	end
	if minv~=nil then
		if nv<minv then nv=minv end
	end
	self:prop(n, nv)
	return nv-old
end

function StdObject:allProp()
	if self._prop then return READONLY(self._prop) end
	return EMPTY_TABLE
end

function StdObject:propArray(p1)
	if self._prop[p1]==nil then
		return self:prop(p1, {})
	end
	return self:prop(p1)
end

function StdObject:hasAttr(n)
	if self._attr == nil then return false end
	return self._attr[n]~=nil
end

function StdObject:removeAttr(n)
	if self._attr == nil then return end
	self:_access(self._attr,n,nil)
end

function StdObject:initAttr(n, v)
	if self._attr~=nil and self._attr[n]~=nil then
		return
	end
	self:attr(n, v)
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

function StdObject:attrArray(p1)
	if self._attr[p1]==nil then
		return self:attr(p1, {})
	end
	return self:attr(p1)
end

function StdObject:hasConst(n)
	if self._const == nil then return false end
	return self._const[n]~=nil
end

function StdObject:const(p1,p2)
	if p2==nil then
		if self._const == nil then return nil end
		return self._const[p1]
	else
		if self._const == nil then self._const = {} end
		self:_access(self._const,p1,p2)
		return p2
	end
end

function StdObject:allConst()
	if self._const then return READONLY(self._const) end
	return EMPTY_TABLE
end

function StdObject:constArray(p1)
	if self._const[p1]==nil then
		return self:const(p1, {})
	end
	return self:const(p1)
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

function StdObject:runvArray(p1)
	if self._runv[p1]==nil then
		return self:runv(p1, {})
	end
	return self:runv(p1)
end

function StdObject:hasWeak(n)
	if self._weak == nil then return false end
	return self._weak[n]~=nil
end

function StdObject:removeWeak(n)
	if self._weak==nil then return end
	self:_access(self._weak,n,nil)
end

function StdObject:weak(p1,p2)
	if p2==nil then
		if self._weak == nil then return nil end
		return self._weak[p1]
	else
		if self._weak == nil then 
			self._weak = {}
			setmetatable(self._weak, {__mode = "v"})
		end
		self:_access(self._weak,p1,p2)
		return p2
	end
end

function StdObject:allWeak()
	if self._weak then return READONLY(self._weak) end
	return EMPTY_TABLE
end

function StdObject:weakArray(p1)
	if self._weak[p1]==nil then
		return self:weak(p1, {})
	end
	return self:weak(p1)
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
	if self.afterLoadObject then
		self:afterLoadObject()
	end
end

function StdObject:_getter(n)
	if n==nil then return nil end
	if n:byte(1)==95 then
		n = n:sub(2)
		return self:attr(n)
	else
		return self:prop(n)
	end	
end

function StdObject:_setter(n, v)
	if n==nil then return nil end
	if n:byte(1)==127 then
		n = n:sub(2)
		self:attr(n, v)
	else
		self:prop(n, v)
	end	
end

function StdObject:_pairs()
	local r = {}
	if self._prop then
		for k, v in pairs(self._prop) do
			r[k] = v
		end
	end
	if self._attr then
		for k, v in pairs(self._attr) do
			r["_"..k] = v
		end
	end	
	return r
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
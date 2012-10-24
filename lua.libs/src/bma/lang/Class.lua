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
	local o = class.new(data._class)
	if o.loadObject ~= nil then
		o:loadObject(data)
	else
		class.deserializeObject(o, data)
	end
	return o
end

local StdObject = class.define("bma.lang.StdObject")

function StdObject:tag(v)
	return self:attr("tag",v)
end

function StdObject:removeThisv(n)
	self[n] = nil
end

function StdObject:thisv(p1,p2)
	if p2==nil then
		return self[p1]
	else
		self[p1] = p2
		return p2
	end
end

function StdObject:removeProp(n)
	if self.properties==nil then return end
	self.properties[n] = nil
end

function StdObject:prop(p1,p2)
	if p2==nil then
		if self.properties == nil then return nil end
		return self.properties[p1]
	else
		if self.properties == nil then self.properties = {} end
		self.properties[p1] = p2
		return p2
	end
end

function StdObject:removeAttr(n)
	if self.attributes==nil then return end
	self.attributes[n] = nil
end

function StdObject:attr(p1,p2)
	if p2==nil then
		if self.attributes == nil then return nil end
		return self.attributes[p1]
	else
		if self.attributes == nil then self.attributes = {} end
		self.attributes[p1] = p2
		return p2
	end
end

function StdObject:removeRunv(n)
	if self.runtime==nil then return end
	self.runtime[n] = nil
end

function StdObject:runv(p1,p2)
	if p2==nil then
		if self.runtime == nil then return nil end
		return self.runtime[p1]
	else
		if self.runtime == nil then self.runtime = {} end
		self.runtime[p1] = p2
		return p2
	end
end

function StdObject:removeTempv(n)
	if self.temp==nil then return end
	self.temp[n] = nil
end

function StdObject:tempv(p1,p2)
	if p2==nil then
		if self.temp == nil then return nil end
		return self.temp[p1]
	else
		if self.temp == nil then self.temp = {} end
		self.temp[p1] = p2
		return p2
	end
end

function StdObject:saveObject()
	local r = {}
	if self.attributes~=nil then
		r.attributes = class.serializeValue(self.attributes)
	end
	if self.properties~=nil then
		r.properties = class.serializeValue(self.properties)
	end
	if self.runtime~=nil then
		r.runtime = class.serializeValue(self.runtime)
	end
	return r
end

function StdObject:loadObject(data)
	if data.attributes~=nil then
		self.attributes = class.deserializeValue(data.attributes)
	end
	if data.properties~=nil then
		self.properties = class.deserializeValue(data.properties)
	end
	if data.runtime~=nil then
		self.runtime = class.serializeValue(data.runtime)
	end
end

if table.filter==nil then table.filter = {} end
table.filter.BY_ATTR = function(n,v)
	return function(k,o)
		if type(o)=="table" and o.attr~=nil then
			return o:attr(n)==v
		end
		return false
	end
end
table.filter.BY_PROP = function(n,v)
	return function(k,o)
		if type(o)=="table" and o.prop~=nil then
			return o:prop(n)==v
		end
		return false
	end
end
table.filter.BY_RUNV = function(n,v)
	return function(k,o)
		if type(o)=="table" and o.runv~=nil then
			return o:runv(n)==v
		end
		return false
	end
end
table.filter.BY_TEMPV = function(n,v)
	return function(k,o)
		if type(o)=="table" and o.tempv~=nil then
			return o:tempv(n)==v
		end
		return false
	end
end
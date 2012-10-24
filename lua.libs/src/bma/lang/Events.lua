-- bma/lang/Event.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.lang.Events")

function Class:ctor()
	self.listeners = {}
end

function Class:listen(lis, id)
	if id == nil then id = tostring(lis) end
	for _,old in ipairs(self.listeners) do
		if old.f == lis then
			return false
		end
		if old.k == id then
			old.f = lis
			return true
		end
	end
	table.insert(self.listeners,{f=lis,k=id})
	return true
end

function Class:sub(filter, id)
	local o = Events.new()
	self:listen(function(...)
		if V(filter(...),false) then
			o.fire(...)
		end
	end,id)
	return o
end

function Class:removeById(id)
	for i,old in ipairs(self.listeners) do
		if old.k == id then
			table.remove(self.listeners,i)
			return true
		end
	end
	return false
end

function Class:remove(lis)
	for i,old in ipairs(self.listeners) do
		if old.f == lis then
			table.remove(self.listeners,i)
			return true
		end
	end
	return false
end

function Class:size()
	return #self.listeners
end

function Class:clear()
	if #self.listeners > 0 then
		self.listeners = {}
	end
end

function Class:fire(...)
	local r = nil
	local rmlist = nil
	for i,lis in ipairs(self.listeners) do
		local r1,r2 = lis.f(...)
		if r1 then
			if rmlist == nil then rmlist = {} end
			table.insert(rmlist,i)
		end
		if r2 ~= nil then
			r = r2
		end
	end
	if rmlist ~= nil then
		for _,i in ipairs(rmlist) do
			table.remove(rmlist, i)
		end
	end
	return r
end



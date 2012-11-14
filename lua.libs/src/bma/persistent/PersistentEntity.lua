-- bma/persistent/PersistentEntity.lua
require("bma.lang.Entity")

-- << class - PersistentEntity >>
local Entity = bma.lang.Entity
local PersistentEntity = class.define("bma.persistent.PersistentEntity", {Entity})
local NF = function()end
local EM = bma.lang.EntityManager

-- abstract
function PersistentEntity:initObject()	
end 

-- abstract
function PersistentEntity:isObjectValid()
	error(self.className.." not implements")
end

function PersistentEntity:id(v)
	if v then self._id = v end
	return self._id
end

function PersistentEntity:loadProp(cb,n,id,param,syn)
	local m = class.instance("bma.persistent.Service")
	local cb1 = function(err, o)
		if not err then
			if o and o:isObjectValid() then
				self:prop(n , o)
				return aicall.done(cb, nil, self)
			end
			cb("loadProp("..tostring(n)..","..tostring(id)..") invalid object")
		else
			cb(err)
		end
	end
	m:load(cb1, id, param,syn)
end

function PersistentEntity:save()
	local m = class.instance("bma.persistent.Service")
	m:save(NF,self:id(),EM.syn)
end

function PersistentEntity:storeObject(cb)
	local m = class.instance("bma.persistent.Service")
	local cb1 = function(err, done)
		cb(nil, {id=self:id()})
	end
	return m:save(cb1, self:id(),EM.syn)	
end

function PersistentEntity:restoreObject(cb, data)
	local m = class.instance("bma.persistent.Service")
	local cb1 = function(err, done)
		if LOG:debugEnabled() then
			LOG:debug("PersistentEntity","[%s,%s] restore from load",self.className, tostring(data.id))
		end
		cb(nil, self)
	end	
	return m:load(cb1, data.id, self, EM.syn)	
end

function PersistentEntity:saveState(cb, syn)	
	local cb1 = function(err, data)
		if not err then
			self:stateModify(false)
		end
		cb(err, data)
	end	
	local tmp = EM.syn
	EM.syn = syn
	local done,r = pcall(function()
		Entity.storeObject(self, cb1)
	end)
	EM.syn = tmp
	if not done then
		error(r)
	end
	return true
end

function PersistentEntity:loadState(cb, objState, syn)
	local tmp = EM.syn
	EM.syn = syn
	local done,r = pcall(function()
		Entity.restoreObject(self, cb, objState)
	end)
	EM.syn = tmp
	if not done then
		error(r)
	end
end

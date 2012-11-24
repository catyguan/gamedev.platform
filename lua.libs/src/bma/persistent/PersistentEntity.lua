-- bma/persistent/PersistentEntity.lua
require("bma.lang.Entity")

-- << class - PersistentEntity >>
local Entity = bma.lang.Entity
local PersistentEntity = class.define("bma.persistent.PersistentEntity", {Entity})

function PersistentEntity.instanceFactory(cls, data)
	local m = class.instance("bma.persistent.Service")
	return m:get(data.id, cls.className)
end

function PersistentEntity:_access(t,n,v)
	if t==self._prop or t==self._attr then
		if t[n]~=v then
			self:stateModify(true)
		end
	end
	t[n] = v
end

function PersistentEntity:isStateModify()
	if self._sM~=nil then
		return self._sM
	end
	return false
end

function PersistentEntity:stateModify(b)
	self._sM = b;
end

-- abstract
function PersistentEntity:initObject()	
	
end 

function PersistentEntity:saveObject()
	return {id=self:id()}
end

function PersistentEntity:loadObject(data)
	if not self._attr then self._attr = {} end
	self._attr.id = data.id
end

function PersistentEntity:storeObject(cb, syn)	
	local m = class.instance("bma.persistent.Service")
	m:save(cb, self:id(), syn)
end

function PersistentEntity:_restoreObject(cb, syn)
	local m = class.instance("bma.persistent.Service")
	local cb1 = function(err, done)
		if err then
			aicall.done(cb, err)
			return
		end
		Entity._restoreObject(self, cb, syn)	
	end	
	return m:load(cb1, self:id(), syn)	
end

function PersistentEntity:saveState()
	return Entity.saveObject(self)
end

function PersistentEntity:loadState(state)
	Entity.loadObject(self, state)
end

function PersistentEntity:unregister(save)
	local S = class.instance("bma.persistent.Service")
	local fn = function(err, done)
		S:unregister(self:id())
	end
	
	if save then
		self:storeObjeect(fn, false)
	else
		fn(nil, true)
	end	
end

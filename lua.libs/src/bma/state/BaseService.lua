-- bma/state/BaseService.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")

-- << class - BaseService >>
local Class = class.define("bma.state.BaseService",{})

function Class:ctor()
	self.objects = {}
end

function Class:register(obj, v)
	local id = obj:id()
	if id==nil then
		error("id invalid")
	end
	if self.objects[id] then
		error("object["..id.."] exists")
	end
	self.objects[id] = {o=obj, v=v }
end

function Class:load(callback, id, type, syn)
	local item = self.objects[id]	
	if item==nil then
		if type==nil then
			error("invalid object["..id.."] and type")
		end
		local obj = class.new(type)
		item = {o=obj, v=0}
		self.objects[id] = item
	end
	local loading = false
	if item.l then
		loading = true
	else
		item.l = {}	
	end
	table.insert(item.l, callback)
	
	if loading and not syn then
		return
	end
	
	local cb = function(err, data, version)
		local item = self.objects[id]		
		if not item then
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "skip load object [%s]", tostring(id))
			end
			return
		end
				
		if err then
			LOG:error("StatefulObjectManager", "load object [%s] fail - %s", tostring(id), err)
		else
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "query object [%s] done - %s", tostring(id), string.dump(data))
			end			
			if data then
				local state = string.json(data)
				item.o:loadState(state)
				item.v = version or 0
			else
				local state = {}
				item.o:initState(state)
				local cb2 = function(err)
					if err then
						if LOG:warnEnabled() then
							LOG:warn("StatefulObjectManager", "init object[%s] data fail - %s", tostring(id), err)
						end
					end
				end
				item.v = self:initObjectData(cb2, LUA_APP_ID, LUA_APP_TYPE, id, table.json(state), false)
			end
			
		end
		
		if item.l then
			for _, c in ipairs(item.l) do
				local done,r1,r2 = pcall(function() 
					c(err, item.o)
				end)
				if not done and LOG:debugEnabled() then
					LOG:debug("StatefulObjectManager", "load object callback fail - %s\n%s", tostring(r1), tostring(r2))
				end
			end
		end
		item.l = nil
	end
	
	self:loadObjectData(cb, LUA_APP_ID, LUA_APP_TYPE, id, syn)	
end

function Class:save(callback, id, syn)
	local item = self.objects[id]	
	if item==nil then
		return false
	end
		
	local saving = false
	if item.s then
		saving = true
	else
		item.s = {}	
	end
	table.insert(item.s, callback)
	
	if saving and not syn then
		return true
	end
	
	local cb = function(err, done)
		local item = self.objects[id]		
		if not item then
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "skip save object [%s]", tostring(id))
			end
			return
		end
				
		if err then
			LOG:error("StatefulObjectManager", "save object [%s] fail - %s", tostring(id), err)
		else
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "save object [%s] done - %s", tostring(id), tostring(done))
			end			
		end
		
		if item.s then
			for _, c in ipairs(item.s) do
				local done,r1,r2 = pcall(function() 
					c(err, item.o)
				end)
				if not done and LOG:debugEnabled() then
					LOG:debug("StatefulObjectManager", "save object callback fail - %s\n%s", tostring(r1), tostring(r2))
				end
			end
		end
		item.s = nil
	end
	
	local state = {}
	item.o:saveState(state)
	item.o:stateModify(false)
	item.v = self:saveObjectData(cb, LUA_APP_ID, LUA_APP_TYPE, id, table.json(state), item.v, syn)
	return true	
end

function Class:saveAll(syn)
	local fn = function()end
	for k,item in pairs(self.objects) do
		if item.o:isStateModify() then
			self:save(fn,k,syn)
		else
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "skip save object[%s]", tostring(k))
			end
		end
	end
end

function Class:delete(id, syn)
	local item = self.objects[id]	
	if item==nil then
		return false
	end
			
	local cb = function(err, done)
		if err then
			LOG:error("StatefulObjectManager", "delete object [%s] fail - %s", tostring(id), err)
		else
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "delete object [%s] done - %s", tostring(id), tostring(done))
			end
		end
	end
	
	self.objects[id] = nil
	self:deleteObjectData(cb, LUA_APP_ID, LUA_APP_TYPE, id, syn)
	return true
end

function Class:deleteAll(syn)
	local cb = function(err, done)
		if err then
			LOG:error("StatefulObjectManager", "delete all object [%s] fail - %s", tostring(id), err)
		else
			if LOG:debugEnabled() then
				LOG:debug("StatefulObjectManager", "delete all object [%s] done - %s", tostring(id), tostring(done))
			end
		end
	end
	
	self.objects = {}
	self:deleteAllObjectData(cb, LUA_APP_ID, LUA_APP_TYPE, syn)
	return true
end
-- bma/state/BaseService.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")

-- << class - BaseService >>
local Class = class.define("bma.state.BaseService",{})
local CFG = CONFIG.StateService or {}
local timerInterval = CFG.timerInterval or 60000
local queueRunTask = CFG.queueRunTask or 20

function Class:ctor()
	self.objects = {}
	self.queue = {}
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

function Class:unregister(id)
	local item = self.objects[id]
	if item.l then
		for _, c in ipairs(item.l) do
			local done,r1,r2 = pcall(function() 
				c("unregister")
			end)			
		end
		item.l = nil
	end
	
	if item.s then
		for _, c in ipairs(item.s) do
			local done,r1,r2 = pcall(function() 
				c("unregister")
			end)			
		end
		item.s = nil
	end	
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
				item.v = self:initObjectData(cb2, id, table.json(state), false)
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
	
	self:loadObjectData(cb, id, syn)	
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
	item.v = self:saveObjectData(cb, id, table.json(state), item.v, syn)
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
	self:deleteObjectData(cb, id, syn)
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
	self:deleteAllObjectData(cb, syn)
	return true
end

function Class:schedule(id)
	if not self.objects[id] then return false end
	for _, value in pairs(self.queue) do
    	if value == id then
      		return true 
		end
  	end
	table.insert(self.queue, id)
	if self.timerId then
		return true
	end
	local HS = class.instance("bma.host.Service")
	self.timerId = HS:setFixTimer(function() self:onTimer() end, timerInterval)
end

function Class:unschedule(id)
	for i, value in pairs(self.queue) do
    	if value == id then
      		table.remove(self.queue, i)
      		local HS = class.instance("bma.host.Service")
      		if #self.queue==0 and self.timerId then
				HS:removeTimer(self.timerId)
			end
			return true
		end
  	end
  	return false
end

function Class:onTimer()
	if LOG:debugEnabled() then
		LOG:debug("StatefulObjectManager", "timer active")
	end
	self.timerId = 0
	
	local c = 1
	local s = self.workAt or 1
	local i = s
	
	while true do
		local id = self.queue[i]
		if self.objects[id] then			
			if self.objects[id].o:isStateModify() then
				if LOG:debugEnabled() then
					LOG:debug("StatefulObjectManager", "timer save - %s", tostring(id))
				end
				self:save(function()end, id)
			else
				if LOG:debugEnabled() then
					LOG:debug("StatefulObjectManager", "timer skip - %s", tostring(id))
				end
			end
			i = i + 1
		else
			table.remove(self.queue, i)
		end
		
		c = c + 1
		if i>#self.queue then i = 1 end
		if i==s or c>queueRunTask then
			self.workAt = i
			break
		end
	end	
	
	return #self.queue>0
end

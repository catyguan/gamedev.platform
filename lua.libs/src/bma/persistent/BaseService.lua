-- bma/persistent/BaseService.lua
require("bma.lang.Entity")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")

-- << class - BaseService >>
local Class = class.define("bma.persistent.BaseService",{})
local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.PersistentService or {}
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

function Class:get(callback, id, type, syn)
	local item = self.objects[id]
	if item and item.ld then 
		aicall.done(callback, nil, item.o)
		return true
	end
	return self:load(callback, id, type, syn)
end

function Class:load(callback, id, param, syn)	
	local item = self.objects[id]	
	if item==nil then
		if type==nil then
			error("invalid object["..id.."] and type")
		end
		local obj
		if type(param)=="string" then 
			obj = class.new(param)
		else
			obj = param
		end
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
		return false
	end
	
	local cb = function(err, data, version)
		local item = self.objects[id]		
		if not item then
			if LDEBUG then
				LOG:debug("PersistentService", "skip load object [%s]", tostring(id))
			end
			return
		end
		
		local loaded = function(err)
			if LDEBUG then
				LOG:debug("PersistentService", "object [%s] loaded - err(%s)", tostring(id), tostring(err))
			end
			item.ld = true
			item.o:id(id)
			if item.l then
				for _, c in ipairs(item.l) do
					aicall.safeDone(c, err, item.o)					
				end
			end
			item.l = nil			
		end
		
		if err then
			LOG:error("PersistentService", "load object [%s] fail - %s", tostring(id), err)
			loaded(err)
		else
			if LDEBUG then
				LOG:debug("PersistentService", "query object [%s] done - %s", tostring(id), string.dump(data))
			end			
			if data then
				local state = string.json(data)
				item.v = version or 0
				state["id"] = nil				
				item.o:loadState(loaded, state, syn)
			else
				item.o:initObject()
				local sscb = function(err, state)
					if err then
						loaded(err)
					else
						item.v = self:initObjectData(loaded, id, table.json(state), false)
					end
				end
				item.o:saveState(sscb, syn)				
			end			
		end
	end
	
	return self:loadObjectData(cb, id, syn)	
end

function Class:save(callback, id, syn)
	local item = self.objects[id]	
	if item==nil then
		return callback("invalid id["..tostring(id).."]")
	end
		
	local saving = false
	if item.s then
		saving = true
	else
		item.s = {}	
	end
	table.insert(item.s, callback)
	
	if saving and not syn then
		return false
	end
	
	local cb = function(err, done)
		local item = self.objects[id]		
		if not item then
			if LDEBUG then
				LOG:debug("PersistentService", "skip save object [%s]", tostring(id))
			end
			return
		end
				
		if err then
			LOG:error("PersistentService", "save object [%s] fail - %s", tostring(id), err)
		else
			if LDEBUG then
				LOG:debug("PersistentService", "save object [%s] done - %s", tostring(id), tostring(done))
			end			
		end
		
		if item.s then
			for _, c in ipairs(item.s) do
				aicall.safeDone(c,err,item.o)				
			end
		end
		item.s = nil
	end
	
	local sscb = function(err, state)
		if err then
			cb(err)
		else
			item.v = self:saveObjectData(cb, id, table.json(state), item.v, syn)		
		end
	end	
	
	return item.o:saveState(sscb, syn)	
end

function Class:saveAll(syn)
	local fn = function()end
	for k,item in pairs(self.objects) do
		if item.o:isStateModify() then
			self:save(fn,k, syn)
		else
			if LDEBUG then
				LOG:debug("PersistentService", "skip save object[%s]", tostring(k))
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
			LOG:error("PersistentService", "delete object [%s] fail - %s", tostring(id), err)
		else
			if LDEBUG then
				LOG:debug("PersistentService", "delete object [%s] done - %s", tostring(id), tostring(done))
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
			LOG:error("PersistentService", "delete all object [%s] fail - %s", tostring(id), err)
		else
			if LDEBUG then
				LOG:debug("PersistentService", "delete all object [%s] done - %s", tostring(id), tostring(done))
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
	if LDEBUG then
		LOG:debug("PersistentService", "timer active")
	end
	self.timerId = 0
	
	local c = 1
	local s = self.workAt or 1
	local i = s
	
	while true do
		local id = self.queue[i]
		if self.objects[id] then			
			if self.objects[id].o:isStateModify() then
				if LDEBUG then
					LOG:debug("PersistentService", "timer save - %s", tostring(id))
				end
				self:save(function()end, id)
			else
				if LDEBUG then
					LOG:debug("PersistentService", "timer skip - %s", tostring(id))
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

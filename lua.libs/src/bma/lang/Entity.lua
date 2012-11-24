-- bma/lang/Entity.lua
require("bma.lang.Class")
require("bma.lang.ai.Core")

local LDEBUG = LOG:debugEnabled()
local LTAG = "Entity"

-- <<class EntityRef>>
local EntityRef = class.define("bma.lang.EntityRef")

-- interface
-- function EntityRef:target() : ? extends Entity

function EntityRef:saveObject()
	return self._target
end

function EntityRef:loadObject(data)		
	self._target = data
end

-- <<class Entity>>
local Entity = class.define("bma.lang.Entity",{bma.lang.StdObject})

-- interface
-- function Entity:storeObject(cb, syn)
-- function Entity:beforeRestoreObject(cb)
-- function Entity:afterRestoreObject(cb)
-- function Entity:refEntity() : ? extends EntityRef

function Entity:restoreObject(cb, syn)
	if self:runv("entityRestored") then
		aicall.done(cb, nil, true)
		return		
	end	
	
	if self.beforeRestoreObject then
		pcall(function()
			self:beforeStoreObject()
		end)
	end
	local cb1 = function(err, done)
		if not err then
			self:runv("entityRestored", true)
			if LDEBUG then
				LOG:debug(LTAG, "Entity[%s] restored", tostring(self:id()))
			end
			if self.afterRestoreObject then
				pcall(function()
					self:afterRestoreObject()
				end)
			end
		end	
		local cbl = self:runv("loadcb") 
		if cbl then
			for _, callback in ipairs(cbl) do
				aicall.safeDoneL(callback, nil, err, done)
			end
			self:removeRunv("loadcb")
		end
	end
	
	local restoring = false
	local cbl = self:runv("loadcb")
	if not cbl then 
		cbl = {}
		self:runv("loadcb",cbl)
	else
		restoring = true
	end
	if cb then
		table.insert(cbl, cb)
	end	
	if restoring and not syn then
		return
	end 
	return self:_restoreObject(cb1, syn)
end

local AIG = bma.lang.ai.Group

-- self class used or overwrite
function Entity:_restoreObject(cb, syn)
	local cb1 = function(err,g)
		local r = true
		if not err then
			local t = g:toTable()
			if t then
				for k, v in pairs(t) do
					if not v then 
						r = false
						if LDEBUG then
							LOG:debug(LTAG,"restore object[%s] data[%s] fail", tostring(self:id()), tostring(k))
						end
					end
				end
			end
		else
			if LDEBUG then
				LOG:debug(LTAG,"restore object[%s] fail - %s", tostring(self:id()), tostring(err))
			end
			r = false
		end
		aicall.done(cb, err, r)
	end
	
	local g = AIG.new(cb1, AIG.logError(LTAG, "restoreObject fail - %s"))
	local rfun = function(k, v)
		print("SV-T-I: "..k.."="..string.dump(v))
		if type(v)=="table" and v.restoreObject then
			v:restoreObject(g:call(k), syn)			
		end
	end
	-- print("SV-T: "..string.dump(data))
	if self._prop then
		for k,v in pairs(self._prop) do			
			rfun(k, v)			
		end
	end
	if self._attr then
		for k,v in pairs(self._attr) do
			rfun(k, v)
		end
	end
	g:finishAll()
end





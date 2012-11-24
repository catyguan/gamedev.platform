-- bma/matrix/Idle.lua
require("bma.lang.Class")

local LDEBUG = LOG:debugEnabled()
local LTAG = "Idle"

-- <<class Idle>>
local Class = class.define("bma.matrix.Idle")

function Class:idleActive()
	self:runv("idleActiveTime", os.mclock())
end

function Class:isIdle()
	local mt = self:runv("maxIdleTime")
	if mt then		
		local la = self:runv("idleActiveTime") or 0
		return os.mclock()- la > mt
	end
	return false
end

function Class:setIdleChecker(fn, t, mt)
	if mt then
		self:runv("maxIdleTime", mt)
	end
	
	local HS = class.instance("bma.host.Service")
	if self:runv("idleTimerId") then
		HS:removeTimer(self:runv("idleTimerId"))
		self:removeRunv("idleTimerId")			
	end		
			
	if t and t>0 then		
		self:idleActive()
		local tid = HS:setFixTimer(function()
				if self:isIdle() then
					fn()
				end 
			end, t)
		self:runv("idleTimerId", tid)
	end
	
end


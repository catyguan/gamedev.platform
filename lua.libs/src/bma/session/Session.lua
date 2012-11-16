-- bma/session/Core.lua
require("bma.lang.Entity")
require("bma.lang.ext.Dump")

local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.Session or EMPTY_TABLE
local LTAG = "Session"

-- <<class Session>>
local Session = class.define("bma.session.Session", {bma.lang.Entity})

function Session:value(n, v)
	self:lastAccessedTime(os.time())
	return self:prop(n, v)
end

function Session:creationTime(v)
	return self:prop("creationTime", v)
end

function Session:sessionId(v)
	return self:prop("sessionId", v)
end

function Session:lastAccessedTime(v)
	return self:prop("lastAccessedTime", v)
end

function Session:active()
	self:lastAccessedTime(os.time())
end

function Session:isTimeout()	
	local mv = self:maxInactiveInterval()
	if mv then
		local la = self:lastAccessedTime() or 0
		return os.time()-la>mv
	end
	return false
end

function Session:maxInactiveInterval(v)
	self:prop("maxInactiveInterval", v)
	self:setTimeoutChecker(v)	
end

function Session:setTimeoutChecker(v)
	local HS = class.instance("bma.host.Service")
	if self:runv("timerId") then
		HS:removeTimer(self:runv("timerId"))
		self:removeRunv("timerId")			
	end		
	
	if self:runv("start") then		
		if v and v>0 then
			if v<10 then v = 10 end
			if v>60 then v = 60 end
			local tid = HS:setFixTimer(function()
					if self:isTimeout() then
						local sid = self:sessionId()
						if LDEBUG then
							LOG:debug(LTAG, "Session[%s] timeout", tostring(sid))
						end
						self:setTimeoutChecker()
						class.instance("bma.session.Service"):deleteSession(sid)
					end 
				end, v*1000)
			self:runv("timerId", tid)
		end
	end
end

function Session:initSession()
	self:setTimeoutChecker()
	if self._p then
		local sid = self:sessionId()
		self._p = {}
		self:sessionId(sid)
		self:creationTime(os.time())
		self:lastAccessedTime(os.time())
	end	
end

function Session:start()
	self:runv("start", true)
	self:setTimeoutChecker(self:maxInactiveInterval())
end

function Session:delete()	
	self:setTimeoutChecker(0)
	if self._p then
		self._p = nil
		self._r = nil
	end
end


require("bma.lang.LruCache")

local LDEBUG = LOG:debugEnabled()
local LTAG = "Session"

-- <<class - Session>>
local Class = class.define("bma.lang.Session", {bma.lang.LruCache})
local HS = class.instance("bma.host.Service")

function Class:addEntry(entry)
	bma.lang.LruCache.addEntry(self, entry)
	if self.started then
		self:start()
	end
end

function Class:start()
	self.started = true
	if self.timerId then
		return
	end
	if self.maxAge<=0 then return end
	self:checkTimeout()
end

function Class:checkTimeout()
	local rc = 0
	while self.currentItems> 0 do
		local l = self.tail.next_
		local now = os.mclock()
		local tm = l.maxAge - now
		if tm<=0 then
			rc = rc + 1
			self:removeEntry(l)
		else
			if not self.timerId then
				local timer = function()
					self:checkTimeout()
				end
				self.timerId = HS:setTimer(timer, tm)
			end
			break
		end
	end
	
	if LDEBUG and rc>0 then
		LOG:debug(LTAG, "%s clear %d", tostring(self), rc)
	end
end

function Class:stop()
	self.started = nil
	local tid = self.timerId
	if self.timerId then
		self.timerId = nil
		HS:removeTimer(tid)
	end 
end

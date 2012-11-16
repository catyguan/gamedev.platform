-- bma/session/BaseService.lua
require("bma.lang.Class")

local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.Session or EMPTY_TABLE
local LTAG = "Session"

-- <<class BaseService>>
local BaseService = class.define("bma.session.BaseService", {})

function BaseService:ctor()
	self.sessions = {}	
end

-- interface
-- createSessionId()
-- createSession(cb, sessionId)

function BaseService:createSimpleSessionId()
	return os.mclock()..math.random(10000000,99999999)
end

local addSession = function(self, sid, err, session)
	local item = self.sessions[sid]
	if not item then return end
	
	if LDEBUG then
		LOG:debug(LTAG, "addSession(%s,%s,%s)", tostring(sid),tostring(err),tostring(session))
	end
	
	if item.s then 
		if LDEBUG then
			LOG:debug(LTAG, "session[%s] already exists", tostring(sid))
		end
		session = item.s
		err = nil
	else
		item.s = session
		session:start()
	end
	
	if item.l then
		for _,c in ipairs(item.l) do
			aicall.safeDone(c, err, session)
		end
	end
	item.l = nil
	
	if err then
		self.sessions[sid] = nil
	end
end

function BaseService:getSession(cb, sid, create)
	local item
	if sid then
		item = self.sessions[sid]
	end
	if item then
		if item.s then
			if not item.s:isTimeout(defaultMaxInactiveInterval) then
				aicall.done(cb, nil, item.s)
				return true				
			end
			item.s:delete()
			item.s = nil
		end
		if item.l then
			table.insert(item.l, cb)
			return false
		end 
	end
	if create then		
		if not sid then sid = self:createSessionId() end
		if not item then 
			item = {}
			self.sessions[sid] = item
		end
		if not item.l then item.l = {} end 
		table.insert(item.l, cb)
		
		local cb1 = function(err, o)
			addSession(self,sid, err, o)
		end
		if LDEBUG then
			LOG:debug(LTAG, "createSession(%s)", tostring(sid))
		end
		return self:createSession(cb1, sid)
	end
	aicall.done(cb,nil,nil)
	return true
end

function BaseService:deleteSession(sid)
	if not sid then return end
	local item = self.sessions[sid]
	if item and item.s then
		item.s:delete()
		item.s = nil
	end
end

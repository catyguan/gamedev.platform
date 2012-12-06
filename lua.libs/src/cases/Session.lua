-- scene/Session.lua
require("bma.app.AppScene")
require("bma.lang.Session")

local Class = class.define("scene.Session",{bma.app.AppScene})
local LDEBUG = LOG:debugEnabled()
local LTAG = Class.className

function Class:ctor(s)
	self.session = s
end

function Class:getValueCommand(cb, sessionId, key)
	if self:isNull(cb, "sessionId", sessionId) then return end
	if self:isNull(cb, "key", key) then return end
	
	local s = self.session:get(sessionId)
	self.session:touch();
	
	local r
	if s and s[key] then
		r = s[key]
	end
	if LDEBUG then
		LOG:debug(LTAG, "getValue(%s, %s) => %s", tostring(sessionId), tostring(key), tostring(r));
	end
	self:success(cb, r)
end

function Class:setValueCommand(cb, sessionId, key, v)
	if self:isNull(cb, "sessionId", sessionId) then return end
	if self:isNull(cb, "key", key) then return end
	
	local s = self.session:get(sessionId)
	if s then
		s[key] = v
		self.session:touch(sessionId)
	else
		s = {}
		s[key] = v
		self.session:set(sessionId, s)
	end
	if LDEBUG then
		LOG:debug(LTAG, "setValue(%s, %s) => true", tostring(sessionId), tostring(key));
	end
	self:success(cb, true)
end

function Class:removeValueCommand(cb, sessionId, key)
	if self:isNull(cb, "sessionId", sessionId) then return end
	if self:isNull(cb, "key", key) then return end
	
	local s = self.session:get(sessionId)
	if s then
		s[key] = nil
		self.session:touch(sessionId)
	end
	if LDEBUG then
		LOG:debug(LTAG, "removeValue(%s, %s) => true", tostring(sessionId), tostring(key));
	end
	self:success(cb, true)
end

function Class:removeSessionCommand(cb, sessionId)
	if self:isNull(cb, "sessionId", sessionId) then return end
	
	self.session:set(sessionId, nil)
	if LDEBUG then
		LOG:debug(LTAG, "removeSession(%s) => true", tostring(sessionId));
	end
	self:success(cb, true)
end
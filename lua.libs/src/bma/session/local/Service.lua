-- bma/session/local/Service.lua
require("bma.lang.Class")
require("bma.session.BaseService")
require("bma.session.Session")

local LDEBUG = LOG:debugEnabled()
local CFG = CONFIG.Session or EMPTY_TABLE
local LTAG = "Session"

-- <<class Service>>
local Service = class.define("bma.session.local.Service", {bma.session.BaseService})

function Service.install()
	local o = Service.new()
	class.setInstance("bma.session.Service", o)
end

function Service:createSessionId()
	return self:createSimpleSessionId()
end

function Service:createSession(cb, sessionId)
	local cls = class.forName(CFG.SessionClass or "bma.session.Session")
	local s = cls.new()
	s:sessionId(sessionId)
	s:initSession()
	aicall.done(cb,nil,s)
end

-- cases/Auth.lua
require("bma.app.AppCase")

local Class = class.define("cases.Auth",{bma.app.AppCase})
local LDEBUG = LOG:debugEnabled()
local LTAG = Class.className

function Class:ctor(authd)
	self.AUTHD = authd
end

function Class:checkAccountMethod(cb, _, account, passwd)
	if self:isNull(cb, "account", account) then return end
	if self:isNull(cb, "password", passwd) then return end
	
	local f = function(err, r)
		self:pass(cb, err, r)
	end
	self.AUTHD:checkAccount(f, account, passwd)
end

function Class:loginMethod(cb,  _, account, passwd)
	if self:isNull(cb, "account", account) then return end
	if self:isNull(cb, "password", passwd) then return end
	
	local f = function(err, r)
		self:pass(cb, err, r)
	end	
	self.AUTHD:login(f, account, passwd)	
end

function Class:checkTokenMethod(cb, _, token)
	if self:isNull(cb, "token", token) then return end
	
	local f = function(err, r)
		self:pass(cb, err, r)
	end
	self.AUTHD:checkToken(f, token)
end

function Class:logoutMethod(cb, _, userToken)
	if self:isNull(cb, "userToken", userToken) then return end
	
	local f = function(err)		
		self:pass(cb, err, true)
	end
	self.AUTHD:logout(f, userToken)
end
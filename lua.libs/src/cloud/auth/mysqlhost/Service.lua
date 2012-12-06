-- clound/auth/mysqlhost/Service.lua
require("bma.lang.Class")
require("bma.lang.ext.DES56")
require("bma.app.protocol.Common")

local P = bma.app.protocol.Common;

local Class = class.define("cloud.auth.mysqlhost.Service", {})
local LDEBUG = LOG:debugEnabled()
local LTAG = Class.className

local DAL = class.instance("bma.dal.Service")
local CFG = CONFIG.AuthService or EMPTY_TABLE

local dataSource = CFG.dataSource or "auth"
local accountTable = CFG.accountTable or "lua_account"
local tokenTable = CFG.tokenTable or "lua_auth_token"
local passwordKey = CFG.passwordKey or "123456"
local tokenValidSeconds = CFG.tokenValidSeconds or 3600

function Class:ctor()
	
end

function Class:enpass(v)
	local s = tostring(v)
	return s:crypt(passwordKey)
end

function Class:checkAccount(cb, account, passwd)
	local f = function(err, r)
		if LDEBUG then
			LOG:debug(LTAG, "checkAccount(%s) => %s, %s",tostring(account), tostring(err), string.dump(r));
		end
		if err then
			return aicall.done(cb,err)
		end		
		r = r[1]
		if not r then
			return aicall.done(cb,nil, P.mistake("account.notExists",tostring(account).." not exists"))
		end
		local epass = self:enpass(passwd) 		
		if epass ~= r.passwd then
			return aicall.done(cb,nil, P.mistake("account.passwordError",tostring(account).." password error"))
		end	
		if r.status~=1 then
			return aicall.done(cb,nil,P.mistake("account.statusInvalid",tostring(account).." status is "..tostring(r.status)))
		end
		return aicall.done(cb, nil, {authId=r.id})
	end
	local sql = "SELECT id, passwd,status FROM "..accountTable.." WHERE account = ?"	
	local params = {account}
	DAL:execute(f, dataSource, sql, params ,{})
end

function Class:login(cb, account, passwd)
	local f1 = function(err, r1)
		if err then return aicall.done(cb, err) end
		if r1.mistake then
			return aicall.done(cb,nil,r1)
		end		

		local f2 = function(err, r2)
			if err then return aicall.done(cb, err) end
			local token = r2[1].token
		
			local f3 = function(err, r3)
				if err then return aicall.done(cb, err) end
				return aicall.done(cb,nil,{authId=r1.authId,token=token})
			end
			
			local sql = "INSERT INTO "..tokenTable.." VALUES(?,?,DATE_ADD(NOW(),INTERVAL ? SECOND),NOW())"	
			local params = {token, r1.authId, tokenValidSeconds}
			DAL:execute(f3, dataSource, sql, params ,{update=true})
		end
		
		local sql = "SELECT replace(uuid(), '-', '') as token"
		DAL:execute(f2, dataSource, sql, nil ,{})
	end
	self:checkAccount(f1, account,passwd)	
end

function Class:checkToken(cb, token)
	local f = function(err, r)
		if LDEBUG then
			LOG:debug(LTAG, "checkToken(%s) => %s, %s",token, tostring(err), string.dump(r));
		end
		if err then
			return aicall.done(cb,err)
		end		
		r = r[1]
		if not r then
			return aicall.done(cb,nil, P.mistake("token.notExists",tostring(token).." not exists"))
		end
		return aicall.done(cb, nil, {authId=r.account_id})
	end
	local sql = "SELECT account_id FROM "..tokenTable.." WHERE token = ? AND valid_time > NOW()"	
	local params = {token}
	DAL:execute(f, dataSource, sql, params ,{})
end

function Class:logout(cb, token)
	local f = function(err, r)
		if err then return aicall.done(cb, err) end
		return aicall.done(cb,nil,{true})
	end
	
	local sql = "DELETE FROM "..tokenTable.." WHERE token = ?"	
	local params = {token}
	DAL:execute(f, dataSource, sql, params ,{update=true})
end
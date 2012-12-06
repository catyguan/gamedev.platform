-- require("bma.test.bootstrap")
require("bma.lang.ext.Dump")
include("cloud.auth.mysqlhost.Service")

local S = cloud.auth.mysqlhost.Service.new()

if true then
	print(S:enpass("12345678"))
end

-- checkAccount
if false then
	local f = function(err, r)
		print("checkAccountReturn", err, string.dump(r))
	end	
	
	print("start")
	S:checkAccount(f,"test","12345678")
end

-- checkToken
if true then
	local f = function(err, r)
		print("checkTokenReturn", err, string.dump(r))
	end	
	
	print("token")
	S:checkToken(f,"2e658d903dfb11e2891780d8bd29946d")
end

-- login
if true then
	local f = function(err, r)
		print("loginReturn", err, string.dump(r))
	end	
	
	print("login")
	S:login(f,"test","12345678")
end

-- logout
if false then
	local f = function(err, r)
		print("logoutReturn", err, string.dump(r))
	end	
	
	print("logout")
	S:logout(f,"8f3844573dfa11e2891780d8bd29946d")
end
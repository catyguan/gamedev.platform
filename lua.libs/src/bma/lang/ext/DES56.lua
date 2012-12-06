-- bma/lang/ext/DES56.lua
local core = require("des56")

require("bma.lang.ext.String")

local checkKey = function(k)
	local l = #k	
	if l<8 then return k..(string.rep("0",8-l))	end
	if l>8 then return k:sub(1,8) end
	return k
end

string.decrypt = function(m, k)
	k = checkKey(k)
	local v = string.hex2bin(m)
	return core.decrypt(v, k)
end

string.crypt = function(m,k)
	k = checkKey(k)
	local v = core.crypt(m,k)
	return (string.gsub(v, ".", function (c)
		return string.format("%02x", string.byte(c))
	end))
end
  
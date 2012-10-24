-- bma/lang/ext/DES56.lua
local core = require("des56")

require("bma.lang.ext.String")

string.decrypt = function(m, k)
	local v = string.hex2bin(m)
	return core.decrypt(v, k)
end

string.crypt = function(m,k)
	local v = core.crypt(m,k)
	return (string.gsub(v, ".", function (c)
		return string.format("%02x", string.byte(c))
	end))
end
  
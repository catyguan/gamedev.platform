-- bma/lang/ext/MD5.lua
local core = require("md5")

string.md5bin = function(k)
	return core.sum(k)
end

string.md5 = function(k)
	k = core.sum(k)
	return (string.gsub(k, ".", function (c)
		return string.format("%02x", string.byte(c))
	end))
end
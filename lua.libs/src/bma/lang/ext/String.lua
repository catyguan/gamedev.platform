-- bma/lang/String.lua

local function h2b(b)
	if b>=48 and b<=57 then
		return b-48
	end
	if b>=65 and b<=70 then
		return b-65+10
	end
	if b>=97 and b<=102 then
		return b-97+10
	end
	return 0
end

string.hex2bin = function(s)
	local l = #s
	local r = ""
	for i = 1, l, 2 do
		local b1 = string.byte(s, i, i)
		local b2 = string.byte(s, i+1, i+1)
		local n = h2b(b1)*16+h2b(b2)
		r = r..string.char(n)
	end
	return r
end

string.bin2hex = function(s)
	return (string.gsub(s, ".", function (c)
		return string.format("%02x", string.byte(c))
	end))
end

string.trim = function(s)
	return (string.gsub(s, "^%s*(.-)%s*$", "%1"))
end

string.split = function(s,sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	string.gsub(s,pattern, function(c) fields[#fields+1] = c end)
	return fields
end

string.caps = function(s)
	return (string.gsub(s, "(%w)([%w]*)",
	function (l, ls)
		return string.upper (l) .. ls
	end))
end

--- Justify a string.
-- When the string is longer than w, it is truncated (left or right
-- according to the sign of w).
string.pad = function(s, w, p)
	p = string.rep (p or " ", math.abs (w))
	if w < 0 then
		return string.sub (p .. s, w)
	end
	return string.sub (s .. p, 1, w)
end


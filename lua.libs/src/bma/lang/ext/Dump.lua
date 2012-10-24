-- bma/lang/Dump.lua

local _loop = {}

local ldump = nil
ldump = function(value)
	local t = type(value)
	if t=='string' then
		return value
	end

	if t == 'table' then
		local tid = tostring(value)
		if _loop[tid] ~= nil then
			return tid
		end
		_loop[tid] = true

		local str = ''
		local auxTable = {}
		for i, v in pairs(value) do
			if (tonumber(i) ~= i) then
				table.insert(auxTable, i)
			else
				table.insert(auxTable, ldump(i))
			end
		end
		table.sort(auxTable)

		str = str..'{'
		local separator = ""
		local entry = ""
		for i, fieldName in ipairs(auxTable) do
			if ((tonumber(fieldName)) and (tonumber(fieldName) > 0)) then
				entry = ldump(value[tonumber(fieldName)])
			else
				entry = fieldName.." = "..ldump(value[fieldName])
			end
			str = str..separator..entry
			separator = ", "
		end
		str = str..'}'
		return str
	end

	if t == 'function' then
		return '<function>'
	end
	return tostring(value)

end

string.dump = function(value)
	local r = ldump(value)
	_loop = {}
	return r
end


function var_dump(o)
	if o ~= nil then
		if type(o)=="table" and o.dump ~= nil then
			print(o:dump())
			return
		end
	end
	print(string.dump(o))
end
-- bma/lang/ext/Table.lua

table.empty = function(t)
	return not next(t)
end

table.size = function(t)
	local c
	for _ in pairs(t) do
		c = c +1
	end
	return c
end

table.keys = function(t)
	local r = {}
	for i in pairs (t) do
		insert (r, i)
	end
	return r
end

table.values =function(t)
	local r = {}
	for _, v in pairs (t) do
		insert (r, v)
	end
	return r
end

table.invertKV = function(t)
	local r = {}
	for i, v in pairs (t) do
		r[v] = i
	end
	return r
end

table.isArray = function(t)
	if t==nil then return false end
	if #t > 0 then return true end
	return false;
end

table.inArray = function(t,v)
	if t==nil then return false end
	for _,tv in ipairs(t) do
		if tv==v then return true end
	end
	return false
end

table.removeArrayValue = function(t,v,one)
	if t==nil then return false end
	local r = false
	local i = 1
	while i<=#t do
		local tv = t[i]	
		if tv==v then
			table.remove(t,i)
			if one then 
				return true
			else
				r = true
				i = i - 1
			end
		end
		i = i + 1
	end
	return r
end

table.pairs = function(t)
	assert(t)
	local a = table.isArray(t)
	if a then
		return ipairs(t)
	else
		return pairs(t)
	end
end

table.copy = function(t,s , deep)
	if deep == nil then deep = true end
	for k, v in table.pairs(s) do
		if type(v)=="table" and deep then
			v = table.clone(v, deep)
		end
		t[k] = v
	end
end

table.clone = function(t, deep)
	local r = {}
	table.copy(r, t, deep)
	return r
end

table.walk = function(t, f, ...)
	for k,v in table.pairs(t) do
		f(k,v,...)
	end
end

table.walkRemove = function(t, f, ...)
	for k,v in table.pairs(t) do
		local r = f(k,v,...)
		if r~= nil and r then
			table.remove(t,k)
		end
	end
end

table.select = function(t, f, ...)
	local rt = {}
	for k,v in table.pairs(t) do
		local r = f(k,v,...)
		if r~= nil and r then
			table.insert(rt,v)
		end
	end
	return rt
end

table.selectOne = function(t, f, ...)
	for k,v in table.pairs(t) do
		local r = f(k,v,...)
		if r~= nil and r then
			return v
		end
	end
	return nil
end

table.sub = function(t,i,j)
	if t == nil then return nil end
	if i==nil then i = 1 end
	if j==nil then j= #t end
	local r = {}
	for idx=i,j do
		if idx>#t then break end
		table.insert(r,t[idx])
	end
	return r
end

table.pathGet = function(o, name, sp)
	if not o then return nil end
	if not sp then sp = "." end
	local namepath = name:split(sp)
	local l = #namepath
	for i = 1, l-1 do
		local n = namepath[i];
		local child = o[n]
		if child then
			o = child
		else
			return nil
		end
	end
	return o[namepath[l]]
end

table.pathSet = function(o,name,v,sp)
	if not o then return end
	if not sp then sp = "." end
	local namepath = name:split(sp)
	local l = #namepath
	for i = 1, l-1 do
		local n = namepath[i];
		local child = o[n]
		if not child then
			child = {}
			o[n] = child			
		end
		o = child
	end
	o[namepath[l]] = v
end

if table.filter==nil then table.filter = {} end
table.filter.AND = function(...)
	local arglist = arg
	return function(...)
		if arglist.n == 0 then return false end
		for i,v in ipairs(arglist) do
			local r = v(...)
			if not r then
				return false
			end
		end
		return true
	end
end
table.filter.OR = function(...)
	local arglist = arg
	return function(...)
		if arglist.n == 0 then return false end
		for i,v in ipairs(arglist) do
			local r = v(...)
			if r~=nil and r then
				return true
			end
		end
		return false
	end
end
table.filter.NOT = function(f)
	return function(...)
		local r = f(...)
		if r~=nil and r then
			return false
		end
		return true
	end
end
table.filter.BY = function(name, v)
	return function(k,o)
		if type(o)=="table" then
			if o[name] ~= nil and o[name] == v then return true end
		end
		return false
	end
end
table.filter.BY_KEY = function(key)
	return function(k,o)
		return k==key
	end
end
table.filter.BY_TOS = function(v)
	return function(k,o)
		return v==tostring(o)
	end
end


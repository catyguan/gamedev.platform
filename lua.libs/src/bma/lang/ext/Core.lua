-- bma/lang/ext/Core.lua

-- macro
function V(v,d)
	if v == nil then return d end
	return v
end

EMPTY_TABLE = {}
setmetatable(EMPTY_TABLE,{__newindex=
	function(t,k) end
})

-- class
local _class= {}
class = {}
local _instance={}

class.isClass = function(t)
	if t==nil then return false end
	return rawget(t, "super")~=nil
end

class.extendClass = function(cls,super)
	if cls==nil then
		error("class is nil")
	end
	if not class.isClass(cls) then
		error(string.format("%s not a class", tostring(cls)))
	end
	if super==nil then
		return
	end
	local csuper = rawget(cls,"super")
	for _,v in ipairs(super) do
		table.insert(csuper,v)
	end
end

class.define = function(name,super)
	local fields = {}
	local pattern = "([^.]+)"
	string.gsub(name,pattern, function(c) table.insert(fields,c) end)

	if #fields==0 then
		error("name is empty")
	end

	local g = _G
	for i=1,#fields-1 do
		local n = fields[i]
		if g[n]==nil then g[n] = {} end
		g = g[n]
	end

	local class_type = {}
	g[fields[#fields]] = class_type
	_class[name] = class_type

	class_type.ctor=false
	class_type.className = name
	class_type.super={}

	class_type.new=function(...)
		local obj={}
		setmetatable(obj,{ __index=
			function(t,k)
				local r = class_type[k]
				if r== nil then return nil end
				if k=="super" then return nil end
				return r
			end
		})
		do
			local create
			create = function(c,...)
				local super = rawget(c,"super")
				for _,s in ipairs(super) do
					create(s, ...)
				end
				if c.ctor then
					c.ctor(obj,...)
				end
			end
			create(class_type,...)
		end
		return obj
	end

	setmetatable(class_type,{__index=
		function(t,k)
			local super = rawget(t,"super")
			for _,s in ipairs(super) do
				local r = s[k]
				if r~=nil then
					if type(r)=="function" then
						t[k] = r
					end
					return r
				end
			end
			return nil
		end
	})

	class.extendClass(class_type,super)
	return class_type
end


class.forName = function(name)
	local r = _class[name]
	if r~=nil then
		return r
	end
	if class.loader ~= nil then
		class.loader(name)
	else
		require(name)
	end
	return _class[name]
end

class.new = function(name)
	local cls = class.forName(name)
	if cls == nil then
		error(string.format("class[%s] not exists", name))
	end
	return cls.new()
end

class.instance = function(name)
	local o = _instance[name]
	local t = type(o)
	if t=="string" then
		local v = class.new(o)
		class.setInstance(name, v);
		o = v
	elseif t=="nil" then
		local v = class.new(name)
		class.setInstance(name, v);
		o = v
	end
	return o
end

class.setInstance = function(name,o)
	_instance[name] = o
end

class.walk = function(cls,f)
	local r,ret = f(cls)
	if V(r,false) then
		return r, ret
	end
	for _,s in ipairs(cls.super) do
		r,ret = class.walk(s,f)
		if V(r,false) then
			return r, ret
		end
	end
	return false
end

-- enum
function enum( tbl )
	local ret = {}
	for i,v in ipairs(tbl) do
		ret[v] = i
	end

	ret.valueOf = function(i)
		return tbl[i]
	end
	ret.size = function()
		return #tbl
	end
	ret.all = function()
		return tbl
	end

	return ret
end

function include(name)
	package.loaded[name] = nil
	return require(name)
end

return true
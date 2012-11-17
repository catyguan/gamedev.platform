-- bma/lang/ext/Core.lua

-- macro
function V(v,d)
	if v == nil then return d end
	return v
end

READONLY = function(t)
	local r = {}
	setmetatable(r, {
		__newindex=
			function(_,_) end,
		__index=
			function(_,k)
				return t[k]
			end
	})
	return r
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

class.superCall = function(o,cls,name, ...)
	local m = false
	local f = function(c)
		if m then
			local fun = c[name]
			if fun and type(fun)=="function" then
				return true, fun 
			else
				return false
			end
		else
			m = cls==c
		end
		return false
	end
	
	local ocls = class.forName(o.className)
	if not ocls then return end
	
	local done, callFunction = class.walk(ocls, f)
	if done then
		return callFunction(o, ...)
	end	
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

-- aicall
aicall = {}
function aicall.done(cb, err, ...)
	if cb then
		if type(cb)=="function" then return cb(err, ...) end
		if type(cb)=="table" then
			if cb.done then return cb.done(err, ...) end
			if err then
				if cb.failure then return cb.failure(err) end
			else
				if cb.success then return cb.success(...) end
			end
		end	
	end	
end
function aicall.success(cb, ...)
	return aicall.done(cb,nil,...)
end
function aicall.failure(cb, err)
	return aicall.done(cb, err)
end
function aicall.safeDone(cb, err, ...)
	local a = arg
	local done,r = pcall(function()
		return aicall.done(cb, err, unpack(a))
	end)	
	if done then return r end
end
function aicall.safeDoneL(cb,f,err, ...)
	local a = arg
	local done,r,st = pcall(function()
		return aicall.done(cb, err, unpack(a))
	end)	
	if done then 
		return r
	else
		if f then
			f(r,st)
		else
			if LOG:debugEnabled() then
				LOG:debug("aicall","callback fail - %s\n%s", tostring(r), tostring(st))
			end
		end 
	end
end
function aicall.safeSuccess(cb, ...)
	return aicall.safeDone(cb,nil,...)
end
function aicall.safeFailure(cb, err)
	return aicall.safeDone(cb,err)
end

function include(name)
	package.loaded[name] = nil
	return require(name)
end

return true
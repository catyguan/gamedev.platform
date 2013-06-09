-- bma/app/ApplicationBase.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")

local Class = class.define("bma.app.ApplicationBase",{})

function Class.instance()
	return class.instance("application")
end

function Class:install()
	class.setInstance("application", self)
end

function Class.startup()
	local appClassName = "app."..LUA_APP_TYPE..".Application"
	local app = class.new(appClassName)	
	app:install()
	if not app:init() then
		error("application["..appClassName.."] init fail")
	end
end

function Class:ctor()
	self.services = {}
	self.objects = {}
	self.datas = {}
end

function Class:init()	
	return true
end

function Class:close()
	
end

local errorf = function(name, idx)
	local namepath = name:split(".")
	local l = #namepath
	local dmsg = "";
	for i = 1, idx do
		if i~= 1 then dmsg = dmsg .. "." end
		dmsg = dmsg .. namepath[i];
	end
	return dmsg.."("..name..")"
end

function Class:dispatchObjectCall(name,...)
	local namepath = name:split(".")
	local o = self.objects
	local l = #namepath
	for i = 1, l do
		local n = namepath[i];
		local child = o[n]
		if child then
			o = child
		else
			error(errorf(name,i).." not exists")
		end
	end
	return o(...)
end

local copyv
local copyt
copyv = function(v)
	local t = type(v)
	if t=="table" then
		local tmp = {}
		copyt(tmp, v)
		return tmp
	elseif t=="function" then
		return v()
	else
		return v	
	end
end
copyt = function(r, o)
	local data = o
	if o._pairs~=nil then
		data = o:_pairs()
	end
	for k,v in pairs(data) do
		r[k] = copyv(v)
	end	
end

function Class:dispatchDataGet(name)
	local namepath = name:split(".")
	local o = self.datas
	local l = #namepath
	for i = 1, l do		
		local t = type(o)		
		if t~="table" then
			return nil
		end
		
		local n = namepath[i];
		local child
		if o._getter then
			child = o:_getter(n)
		else
			child = o[n]		
		end
		if type(child)=="function" then child = child() end
		if not child then return nil end
		o = child
	end
	
	return copyv(o)
end

function Class:dispatchDataSet(name, val)
	local namepath = name:split(".")
	local o = self.datas
	local l = #namepath
	for i = 1, l-1 do
		local n = namepath[i];		
		local child
		if o._getter~=nil then
			child = o:_getter(n)
		else
			child = o[n]		
		end
		if type(child)=="function" then 
			child = child()			
		end
		if type(child)~="table" then return false end		
		o = child
	end
	
	local n = namepath[l]
	if o._setter~=nil then
		o:_setter(n, val)
	else
		local v = o[n]
		local t = type(v)
		if t=="function" then
			v(true, val)
		else
			o[n] = val
		end	
	end
	return true
end

function DATAHELPER(helper)
	local r = {}
	setmetatable(r, {
		__newindex=
			function(t,k,v)
				if helper.newindex then
					helper.newindex(tb,k,v)
				end
			end,
		__index=
			function(tb,k)
				if helper.index then
					return helper.index(tb,k)
				end
				return rawget(tb,k)
			end
	})
	return r
end

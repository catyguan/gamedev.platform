-- bma/c2dx/CCUserDefault.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.c2dx.CCUserDefault",{})

local HS = class.instance("bma.host.Service")

function Class.getForKey(key, dv)
	return HS:scall("UserDefault_getForKey", key, dv)
end

function Class.setForKey(key, v)
	return HS:scall("UserDefault_setForKey", key, v)
end
-- bma/lang/Singleton.lua
require("bma.lang.ext.Core")

-- <<class part - Singleton>>
local Singleton = class.define("bma.lang.Singleton")

function Singleton.entityFactory(cb, cls, data)
	local o = class.instance(cls.className)
	aicall.done(cb, nil, o)
	return true
end


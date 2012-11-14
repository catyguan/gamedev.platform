-- bma/lang/Singleton.lua
require("bma.lang.ext.Core")

-- <<class part - Singleton>>
local Singleton = class.define("bma.lang.Singleton")

function Singleton.entityFactory(cb, cls, data)
	local o = class.instance(cls.className)
	aicall.done(cb, nil, o)
end

function Singleton:saveObject()
	return EMPTY_TABLE
end

function Singleton:loadObject(data)
	
end

function Singleton:storeObject(cb)
	aicall.done(cb, nil, EMPTY_TABLE)
end

function Singleton:restoredObject(cb, data)
	aicall.done(cb, nil, self)
end
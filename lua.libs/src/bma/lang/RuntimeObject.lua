-- bma/lang/RuntimeObject.lua
require("bma.lang.ext.Core")

-- <<class part - Singleton>>
local RuntimeObject = class.define("bma.lang.RuntimeObject")

function RuntimeObject:saveObject()
	return EMPTY_TABLE
end

function RuntimeObject:loadObject(data)
	
end

function RuntimeObject:storeObject(cb)
	aicall.done(cb, nil, EMPTY_TABLE)
end

function RuntimeObject:restoredObject(cb, data)
	aicall.done(cb, nil, self)
end
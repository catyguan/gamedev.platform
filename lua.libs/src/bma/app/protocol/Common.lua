-- bma/app/protocol/Common.lua
require("bma.lang.Class")

local Class = class.define("bma.app.protocol.Common")

function Class.mistake(code, msg)
	return {mistake=true, code=code, message=msg}
end

function Class.forward(gname, sceneName, cmdName, ...)
	return {forward=true, name=gname, scene=sceneName, command=cmdName, params={...}}
end
-- bma/app/protocol/Common.lua
require("bma.lang.Class")

local Class = class.define("bma.app.protocol.Common")

function Class.mistake(code, msg)
	return {mistake=true, code=code, message=msg}
end

function Class.forward(appId, sceneName, params)
	return {forward=true, app=appId, scene=sceneName, params=params}
end
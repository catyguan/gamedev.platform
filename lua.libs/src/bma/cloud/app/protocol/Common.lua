-- bma/app/protocol/Common.lua
require("bma.lang.Class")

local Class = class.define("bma.app.protocol.Common")

function Class.success(r)
	return {success=true, result=r}
end

function Class.mistake(code, msg)
	return {mistake=true, code=code, message=msg}
end

function Class.error(code, msg)
	return {error=true, code=code, message=msg}
end

function Class.forward(gname, caseName, cmdName, session, ...)
	return {forward=true, name=gname, caseName=caseName, methodName=methodName, session=session, params={...}}
end
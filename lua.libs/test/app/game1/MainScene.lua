-- app/game1/MainScene.lua
require("bma.lang.Class")

local Class = class.define("app.game1.MainScene", {bma.lang.Singleton, bma.app.AppScene})

function Class:createUserCommand(cb, sid, uid, userName)
	local S = class.instance("bma.persistent.Service")
	local user = S:get("user"..uid, "app.game1.User")

	local cb1 = function(err, r)
		if not err then
			user:active()
		end
		aicall.done(cb, err, done)
	end
	user:restoreObject(cb1)	
end

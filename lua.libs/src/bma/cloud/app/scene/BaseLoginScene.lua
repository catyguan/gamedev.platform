-- bma/app/scene/BaseLoginScene.lua
require("bma.lang.Class")

local Class = class.define("bma.app.scene.BaseLoginScene", 
								{"bma.app.scene.BaseServiceScene"})

function Class:loginCommand(cb, sid, uid, userName)
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

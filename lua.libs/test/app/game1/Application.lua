-- app/app2/Application.lua
require("bma.app.StatefulApplication")

local Class = class.define("app.game1.Application",{bma.app.StatefulApplication})

function Class:listMainObject()
	local r = {}
	r["game"] = "app.game1.Game"
	return r
end

function Class:onLaunch(olist)
	LOG:info("game1", "onLaunch")	
	olist.game:start(self)
	return true
end

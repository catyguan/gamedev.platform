-- app/app2/Application.lua
require("bma.app.StatefulApplication")

local Class = class.define("app.app2.Application",{bma.app.StatefulApplication})

function Class:listMainObject()
	local r = {}	
	return r
end

function Class:onLaunch(olist)
	LOG:info("app2", "onLaunch")	
	return true
end

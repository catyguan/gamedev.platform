-- app/app2/Application.lua
require("bma.app.StatefulApplication")

local Class = class.define("app.app2.Application",{bma.app.StatefulApplication})

function Class:init()
	self:onLaunch()	
	return true
end

function Class:onLaunch()
	LOG:info("app2", "onLaunch")	
	return true
end

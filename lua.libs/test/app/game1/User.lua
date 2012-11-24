-- app/game1/Game.lua
require("bma.lang.Class")

local Class = class.define("app.game1.User",{"bma.persistent.PersistentEntity", "bma.matrix.Idle"})

function Class:initObject()
	self:prop("uid", "")
	self:prop("name", "")	
end

function Class:afterRestoreObject()
	local fn = function()
		
	end
	self:setIdleChecker(fn, 60*1000, 5*60*1000)
end


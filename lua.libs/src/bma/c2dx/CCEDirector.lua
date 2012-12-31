-- bma/c2dx/CCEDirector.lua
require("bma.lang.ext.Core")

local Director = class.define("bma.c2dx.CCEDirector",{})
local Actor = class.define("bma.c2dx.CCEActor",{});

local HS = class.instance("bma.host.Service")

function Director.pushScene(name, ...)
	return HS:scall("Director_pushScene", name, ...)	
end

function Director.replaceScene(name, ...)
	return HS:scall("Director_replaceScene", name, ...)
end

function Director.popScene()
	return HS:scall("Director_popScene")
end

function Director.sendCommand(id, name, ...)
	return HS:scall("Director_sendSceneCommand",id, name, ...)
end

function Director.aicall(f, id, name, ...)
	HS:call(f, "Director_sendSceneCommand",id, name, ...)
end

function Director.actor(id)
	return Actor.new(id)
end

function Actor:ctor(id)
	self.id = id
end

function Actor:sendCommand(name, ...)
	Director.sendCommand(self.id,name,...)
end

function Actor:aicall(f,name, ...)
	Director.aicall(f,self.id,name,...)
end

function Actor:statusSwitch(typeName, actionName)
	self:sendCommand("statusSwitch", typeName, actionName)
end
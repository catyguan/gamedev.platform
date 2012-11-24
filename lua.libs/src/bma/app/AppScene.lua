-- bma/app/AppScene.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")

local Class = class.define("bma.app.AppScene")

function Class:ctor()
	self.commands = {}
end

function Class:getCommand(name)
	local s = self.commands[name]
	if s~=nil then return s end
	s = self:createCommand(name)
	if s~=nil then
		self.commands[name] = s
		return s 
	end
	s = self[name.."Command"]
	if s~=nil and type(s)=="function" then
		self.commands[name] = s
		return s
	end
	return nil
end

function Class:createCommand(name)
	return nil
end

function Class:sceneCommandCall(cb, commandName, ...)
	local cmd = self:getCommand(commandName)
	if cmd then
		cmd(self, cb, ...)
	else
		aicall.done(cb, "invalid command ["..commandName.."]")
	end	
	return true 
end
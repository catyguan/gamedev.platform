-- bma/gcommand/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.gcommand.BaseService")

function Class:ctor()
	
end

function Class:getAccessId(name)
	if CONFIG.GCommand then
		local gnm = CONFIG.GCommand.GName
		if gnm then
			if gnm[name] then return gnm[name] end
			if gnm["*"] then return gnm["*"] end
		end
	end
	return nil
end

function Class:command(cb, gName, sceneName, commandName,...)
	local accessId = self:getAccessId(gName)
	local mcb = function(err,r,...)
		if err then
			aicall.done(cb, err, r, ...)
		else
			if r and type(r)=="table" then
				if r.forward then
					self:command(cb, r.name, r.scene, r.command, unpack(r.params))
					return
				end
			end
			aicall.done(cb, err, r, ...)
		end
	end
	
	if accessId==nil then
		local app = bma.app.Application.instance()
		if app then
			return app:sceneCommand(mcb, sceneName, commandName, ...)
		end
		error("invalid application")
	else
		self:processGlobalCommandCall(mcb, accessId, sceneName, commandName, ...)		
	end
end

function gcommandDispatcher(name)
	local S = class.instance("bma.gcommand.Service")
	return S:getAccessId(name)
end

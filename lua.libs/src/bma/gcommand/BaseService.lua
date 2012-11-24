-- bma/gcmd/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.gcmd.BaseService")

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
	if accessId==nil then
		local app = Class.instance()
		if app then
			return app:sceneCommand(cb, sceneName, commandName, ...)
		end
		error("invalid application")
	else
		self:processGlobalCommandCall(cb, accessId, sceneName, commandName, ...)		
	end
end

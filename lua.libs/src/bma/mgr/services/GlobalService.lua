-- bma/mgr/services/GlobalService.lua
require("bma.lang.ext.Json")

local Class = class.define("bma.mgr.services.GlobalService",{"bma.app.ApplicationService"})

local file = application.file

local obj = function(self)
	if self.object==nil then
		self.object = class.new("bma.mgr.services.GlobalObject")
		self.object:loadData()
	end
	return self.object
end

function Class:ctor()	
	self.id = "global"
end

function Class:newProfile(pid)
	return obj(self):newProfile(pid)
end

function Class:curretProfileId()
	return obj(self).currentProfileId
end

function Class:lastProfileId()
	return obj(self).currentProfileId
end

function Class:listProfiles()
	local list = this.object:prop("profiles")
	if list==nil then
		return {}
	end
	return nil
end

function Class:getProfile(pid)
	return obj(self):getProfile(pid)
end

function Class:listProfiles()
	return obj(self):listProfiles()
end
	
function Class:changeProfile(pid)
	return obj(self):changeProfile(pid)
end

function Class:saveProfiles()
	return obj(self):saveData()
end
	
function Class:canDeleteProfile(pid)
	return obj(self):canDeleteProfile(pid)
end
	
function Class:deleteProfile(pid)
	return obj(self):deleteProfile(pid)
end
-- bma/mgr/activities/GlobalActivity.lua
require("bma.lang.ext.Json")

local Class = class.define("bma.mgr.activities.GlobalActivity", {"bma.mgr.ActivityBase"})

local obj = function(self)
	if self._object==nil then
		self._object = class.new("bma.mgr.activities.FileObjectBase")
		self._object:loadData("/global.ldf")
	end
	return self._object
end

function Class:ctor()	
	self.id = "global"
end

function Class:value(n,v, save)	
	local obj = obj(self)
	return obj:value(n, v, save)
end

function Class:save()
	if self._object~=nil then
		return self._object:saveData()
	end	
end
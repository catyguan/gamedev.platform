-- bma/mgr/services/GameService.lua
require("bma.lang.ext.Json")

local SClass = class.define("bma.mgr.services.GameService",{"bma.app.ApplicationService"})
local Class = class.define("bma.mgr.services.GameObject",{"bma.mgr.services.FileObjectBase"})

-- GameObject
function Class:fileName(gid)
	return "/game_"..gid..".ldf"
end

-- GameService
function SClass:ctor()	
	self.id = "game"
end

function SClass:load(gid)
	local o = class.new(self._objectClass)
	local fn = o:fileName(gid)
	o:loadData(fn)
	self._object = o
end

function SClass:save()
	if self._object then
		self._object:saveData()
	end
end
	
function SClass:delete(gid)
	local o = class.new(self._objectClass)
	local fn = o:fileName(gid)
	o:deleteData(fn)
end

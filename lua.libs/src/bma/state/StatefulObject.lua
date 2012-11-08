-- bma/state/StatefulObject.lua
require("bma.lang.Class")

-- << class - StatefulObject >>
local StatefulObject = class.define("bma.state.StatefulObject", {})

-- function StatefulObject:id()
-- function StatefulObject:saveState(objState)
-- function StatefulObject:loadState(objState)

function StatefulObject:isStateModify()
	if self._stateModify~=nil then
		return self._stateModify
	end
	return false
end

function StatefulObject:stateModify(b)
	self._stateModify = b;
end

function StatefulObject:initState(objState)
	self:saveState(objState)
end 

function StatefulObject:save()
	local m = class.instance("bma.state.Service")
	m:save(function()end,self:id(),false)
end

function StatefulObject:saveNow()
	local m = class.instance("bma.state.Service")
	m:save(function()end,self:id(),true)
end

-- bma/app/StatefulApplication.lua
require("bma.app.Application")

local Class = class.define("bma.app.StatefulApplication",{bma.app.Application})

function Class:init()
	local S = class.instance("bma.state.Service")
	local list = self:listMainObject()
	if list and #list>0 then
		local cf = function(idx, o)
			list[idx].done = true
			list[idx].object = o
			for i,item in ipairs(list) do
				if not item.done then return end
			end
			self:onLaunch(list)
		end
		for i,item in ipairs(list) do
			local fn = function(err,o)
				cf(i,o)
			end
			S:load(fn, item.id, item.type, item.syn)
		end
		return true		
	end
	self:onLaunch({})
	return true
end

function Class:close(destroy)
	local S = class.instance("bma.state.Service")
	if destroy then
		S:deleteAll(true)
	else
		S:saveAll(true)
	end
end

-- bma/app/StatefulApplication.lua
require("bma.app.Application")

local Class = class.define("bma.app.StatefulApplication",{bma.app.Application})

function Class:init()
	local S = class.instance("bma.persistent.Service")
	local list = self:listMainObject()
	if list then
		local cf = function(id, o)
			if id then
				list[id].done = true
				list[id].object = o
				o:id(id)
			end
			for _,item in pairs(list) do
				if not item.done then return end
			end
			local rep = {}
			for k,item in pairs(list) do
				rep[k] = item.object
			end
			self:onLaunch(rep)
		end
		for k,item in pairs(list) do
			if type(item)=="string" then 
				item = {type=item}
				list[k] = item 
			end
			local fn = function(err,o)
				cf(k,o)
			end
			S:load(fn, k, item.type, item.syn)
		end
		cf()
		return true		
	end
	self:onLaunch({})
	return true
end

function Class:close(destroy)
	local S = class.instance("bma.persistent.Service")
	if destroy then
		S:deleteAll(true)
	else
		S:saveAll(true)
	end
end

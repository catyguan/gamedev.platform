-- bma/app/StatefulApplication.lua
require("bma.app.Application")
require("bma.lang.ai.Core")

local Class = class.define("bma.app.StatefulApplication",{bma.app.Application})

function Class:restore(cb,list)
	local S = class.instance("bma.persistent.Service")
	if list then
		local cb1 = function(err, g)
			local r
			if g~=nil then
				r = g:toTable()
			end
			aicall.done(cb, err, r)
		end		
		
		local g = bma.lang.ai.Group.new(cb1)		
		for k,item in pairs(list) do
			if type(item)=="string" then 
				item = {type=item}
				list[k] = item 
			end
			
			local o = S:get(k, item.type, item.syn)
			if o then
				o:restoreObject(g:call(k))
			end
		end
		g:finishAll()		
	end
end

function Class:close(destroy)
	local S = class.instance("bma.persistent.Service")
	if destroy then
		S:deleteAll(true)
	else
		S:saveAll(true)
	end
end

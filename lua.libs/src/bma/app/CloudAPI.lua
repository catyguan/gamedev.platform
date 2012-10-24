require("bma.lang.ext.Json")

local app = class.instance("application")

function cloudNodeCall(serviceId, apiId, req, callback)
	-- return type, content
end

function cloudAppGetDesc()
	local desc = app:getDesc()
	if desc==nil then
		desc = {}
	end
	return table.json(desc)			
end

function cloudAppGetService(serviceId)
	-- return table.json(desc) 
end

function cloudAppListService()
	-- return table.json(list<desc>)
end

function cloudServiceGetApi(serviceId, apiId)
	-- return table.json(desc)
end

function cloudServiceListApi(serviceId)
	-- return table.json(list<desc>)
end
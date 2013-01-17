-- bma/matrix/DataStore.lua
require("bma.lang.Class")
require("bma.lang.ext.Json")

-- <<class RoomManager>>
local DataStore = class.define("bma.matrix.DataStore")

function DataSource.textWriter(p)
	local file
	if type(fileName)=="string" then
		file = io:open(p,"r")
	else
		file = p
	end
	return function(data)	
		local s = Json.encode(data)
		file:write(s)
		file:write("\n")
		file:flush()
	end
end

function DataSource.textReader(p)
	local file
	if type(fileName)=="string" then
		file = io:open(p,"r")
	else
		file = p
	end
	return function()
		local s = file:read()
		if s then
			return Json.decode(s)
		end
		return nil
	end
end


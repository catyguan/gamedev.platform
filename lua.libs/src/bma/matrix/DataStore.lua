-- bma/matrix/DataStore.lua
require("bma.lang.Class")
require("bma.lang.ext.Json")

local Json = lang.Json

-- <<class RoomManager>>
local DataStore = class.define("bma.matrix.DataStore")

function DataStore.textWriter(p)
	local this = {p=p}		
	return function(data)	
		local s = Json.encode(data)	
		local file = this.file
		if not file then
			if type(this.p)=="string" then
				file,err = io.open(this.p,"w+")
				if not file then error(err) end
			else
				file = this.p
			end
			this.file = file
		end		
		file:write(s)
		file:write("\n")
		file:flush()
	end
end

function DataStore.textReader(p)
	if not p then return nil end
	
	local this = {p=p}	
	return function()
		local file = this.file
		if not file then
			if type(this.p)=="string" then
				file,err = io.open(this.p,"r")
				if not file then error(err) end
			else
				file = this.p
			end
			this.file = file
		end
		local s = file:read()
		if s then
			return Json.decode(s)
		end
		return nil
	end
end


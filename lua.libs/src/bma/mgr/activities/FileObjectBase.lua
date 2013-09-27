-- bma/mgr/activities/FileObjectBase.lua
require("bma.lang.Class")
require("bma.lang.ext.Json")

local Class = class.define("bma.mgr.activities.FileObjectBase",{})

local LDEBUG = LOG:debugEnabled()
local LTAG = "FileObject"
local file = application.file

function Class:ctor()
	
end

function Class:loadData(fileName)
	local str = file.load(fileName)
	if LDEBUG then
		LOG:debug(LTAG, "load "..fileName.." ==> " .. tostring(str))
	end
	local data
	if str~="" then
		data = string.json(str)
	else
		data = {}
	end	
	class.deserializeObject(self, data)	
	self._fileName = fileName
end

function Class:saveData(fileName)
	if fileName==nil then fileName = self._fileName end
	local data = class.serializeObject(self.className, self, {"_fileName"})
	local str = table.json(data)
	if LDEBUG then
		LOG:debug(LTAG, "save "..fileName.." ==> "..tostring(str))
	end
	file.save(fileName, str)	
end

function Class:deleteData(fileName)
	if fileName==nil then fileName = self._fileName end	
	if LDEBUG then
		LOG:debug(LTAG, "delete "..fileName)
	end
	file.delete(fileName)	
end

function Class:value(n,v, save)	
	if v~=nil or save~=nil then
		-- set
		self[n] = v
		if save==nil then save = true end
		if save then
			self:saveData()
		end
		return nil
	else
		-- get
		return self[n]
	end	
end
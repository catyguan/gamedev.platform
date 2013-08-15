-- bma/mgr/services/GlobalObject.lua
require("bma.lang.Class")
require("bma.lang.ext.Json")

local Class = class.define("bma.mgr.services.GlobalObject",{"bma.lang.StdObject"})

local LDEBUG = LOG:debugEnabled()
local LTAG = "Global"
local file = application.file

-- currentProfileId
-- lastProfileId
-- profiles
function Class:ctor()
	
end

function Class:loadData()
	local str = file.load("/global.ldf")
	if LDEBUG then
		LOG:debug(LTAG, "load global.ldf ==> " .. tostring(str))
	end
	local data
	if str~="" then
		data = string.json(str)
	else
		data = {}
	end	
	self:loadObject(data)	
end

function Class:saveData()
	local data = self:saveObject()	
	local str = table.json(data)
	if LDEBUG then
		LOG:debug(LTAG, "save global.ldf ==> "..tostring(str))
	end
	file.save("/global.ldf", str)	
end

function Class:nextProfileId()
	local plist = self:prop("profiles")
	local maxid = 0
	if plist~=nil then
		for _,profile in ipairs(plist) do
			if profile.id > maxid then maxid = profile.id end
		end
	end
	return maxid+1
end

function Class:newProfile(pid)
	if pid==nil then
		pid = self:nextProfileId()
	end
	local plist = self:prop("profiles")
	if plist==nil then
		plist = {}
		self:prop("profiles", plist)
	end
	table.insert(plist, {id=pid})
	self:saveData()
	return pid
end

function Class:getProfile(pid)
	local plist = self:prop("profiles")
	if plist~=nil then
		for _,profile in ipairs(plist) do
			if profile.id > maxid then return profile end
		end
	end
	return nil
end

function Class:changeProfile(pid)
	self:prop("lastProfileId", self:prop("currentProfileId"))
	self:prop("currentProfileId", pid)	
	self:saveData()
end

function Class:canDeleteProfile(pid)
	if self:prop("currentProfileId") == pid then return false end
	return true
end

function Class:deleteProfile(pid)
	if not self:canDeleteProfile(pid) then
		return false
	end
	
	local plist = self:prop("profiles")
	if plist~=nil then
		for i,profile in ipairs(plist) do
			if profile.id==pid then
				table.remove(plist, i)
				break
			end
		end
		self:saveData()
	end	
	return true
end
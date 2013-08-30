-- bma/mgr/services/ProfileService.lua
require("bma.lang.ext.Json")

local Class = class.define("bma.mgr.services.ProfileService",{"bma.app.ApplicationService"})

local info = function(self)
	if self._object==nil then
		local o = class.new("bma.mgr.services.FileObjectBase")
		o:loadData("/profiles.ldf")
		self._object = o
	end
	return self._object
end

local profileName = function(pid)
	return "/profile_"..pid..".ldf"
end

function Class:ctor()	
	self.id = "profile"
	self._profileObjects = {}
end

function Class:initProfiles(maxNum)
	local o = info(self)
	if o.profiles==nil then o.profiles = {} end
	local tmp = {}
	local r = #o.profiles~=maxNum
	for i=1, maxNum do
		if o.profiles[i]==nil then
			tmp[i] = {}
			r = true
		else
			tmp[i] = o.profiles[i]
		end
	end
	o.profiles = tmp
	if r then
		o:saveData()
	end
end

function Class:listProfileInfo()
	local list = info(self).profiles
	if list==nil then
		return {}
	end
	return nil
end

function Class:updateProfileInfo(pid, n, v, save)
	local o = info(self)
	if pid==nil then
		pid = o.currentProfileId
		if pid==nil then return end
	end
	if o.profiles and o.profiles[pid] then
		local p = o.profiles[pid]
		p[n] = v
		if save then
			o:saveData()
		end
	end
end

function Class:curretProfileId()
	return obj(self).currentProfileId
end

function Class:changeProfile(pid)
	local o = obj(self)
	o:value("currentProfileId", pid, true)	
end

function Class:getProfile()
	local iobj = info(self)
	local cid = iobj.currentProfileId
	if cid==nil or iobj.profiles[cid]==nil then
		return nil
	end
	local pobj = self._profileObjects[cid]
	if pobj==nil then
		local pobj = class.new("bma.mgr.services.FileObjectBase")
		pobj:loadData(profileName(cid))
		self._profileObjects[cid] = pobj
	end
	return pobj
end
	
function Class:saveProfile()
	local pobj = self:getProfile()
	if pobj~=nil then
		pobj:saveData()
		self:updateProfileInfo(nil, "saveTime", os.date(), true)
	end	
end
	
function Class:deleteProfile(pid)
	local iobj = info(self)
	
	if pid==iobj.currentProfileId then
		iobj.currentProfileId = nil
	end
	if iobj.profiles and o.profiles[pid] then
		o.profiles[pid] = {}		
	end
	o:saveData()
	
	local pobj
	if self._profileObjects[pid] then
		pobj = self._profileObjects[pid]
		self._profileObjects[pid] = nil
	else
		pobj = class.new("bma.mgr.services.FileObjectBase")		
	end
	pobj:deleteData(profileName(pid))	
end
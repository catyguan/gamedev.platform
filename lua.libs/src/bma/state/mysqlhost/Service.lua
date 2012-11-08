-- bma/state/mysqlhost/Service.lua
require("bma.lang.ext.Core")
require("bma.state.BaseService")

local DAL = class.instance("bma.dal.Service")
local CFG = CONFIG.StateService or {}
local tableName = CFG.tableName or "lua_stateful_object"

-- << class - Service >>
local Class = class.define("bma.state.mysqlhost.Service",{bma.state.BaseService})
local instance;

function Class.install()
	if not instance then instance = Class.new() end	
	class.setInstance("bma.state.Service", instance)
end

function Class:initObjectData(callback, appId, appType, objId, data, syn)
		
	local cb = function(err, r)		
		if err then			
			callback(err)
		else			
			callback(nil,true)				
		end
	end
	
	local sql = "INSERT IGNORE INTO "..tableName.." VALUES(?,?,?,?,NOW(),NOW(),1)"	
	local params = {appId, appType, objId, data}
	
	DAL:execute(cb, "state", sql, params ,{update=true, syn=syn})
	return 1
end

function Class:loadObjectData(callback, appId, appType, objId, syn)
		
	local cb = function(err, r)		
		if err then			
			callback(err)
		else			
			if r and r[1] then
				r = r[1]
				callback(nil, r.data, r.version)
			else
				callback(nil,nil,nil)
			end	
		end
	end
	
	local sql = "SELECT * FROM "..tableName.." WHERE app_id = ? AND app_type = ? AND object_id = ?"	
	local params = {appId, appType, objId}
	
	DAL:execute(cb, "state", sql, params ,{syn=syn})
end

function Class:saveObjectData(callback, appId, appType, objId, data, version, syn)
		
	local cb = function(err, r)		
		if err then			
			callback(err)
		else			
			callback(nil,r==1)				
		end
	end
	
	local ver = version+1
	local sql = "UPDATE "..tableName.." SET data=?, version=?, last_update_time=NOW() WHERE app_id = ? AND app_type = ? AND object_id = ? AND version<?"	
	local params = {data, ver, appId, appType, objId, ver}
	
	DAL:execute(cb, "state", sql, params ,{update=true, syn=syn})
	return ver
end

function Class:deleteObjectData(callback, appId, appType, objId, syn)
		
	local cb = function(err, r)		
		if err then			
			callback(err)
		else			
			callback(nil,r==1)				
		end
	end
	
	local sql = "DELETE FROM "..tableName.." WHERE app_id=? AND app_type=? AND object_id=?"	
	local params = {appId, appType, objId}	
	DAL:execute(cb, "state", sql, params ,{update=true, syn=syn})
end

function Class:deleteAllObjectData(callback, appId, appType, syn)
		
	local cb = function(err, r)		
		if err then			
			callback(err)
		else			
			callback(nil,r)				
		end
	end
	
	local sql = "DELETE FROM "..tableName.." WHERE app_id=? AND app_type=?"	
	local params = {appId, appType}	
	DAL:execute(cb, "state", sql, params ,{update=true, syn=syn})
end
-- bma/dal/host/Service.lua
require("bma.lang.ext.Core")
require("bma.dal.BaseService")

local Class = class.define("bma.dal.host.Service", {bma.dal.BaseService})

function Class.install()
	local o = Class.new()
	class.setInstance("bma.dal.Service", o)
end

function Class:ctor()
	
end

-- function execute(callStack, dsName:string, sql:string, params:list, opts:map)
-- records:int or resultSet:list<map>
function Class:execute(callback, dsName, sql, params, executeOpts)
	local HS = class.instance("bma.host.Service")
	local dsname = self:mapDSName(dsName)
    return HS:call(callback, "dal.execute", dsname, sql, params, executeOpts)
end
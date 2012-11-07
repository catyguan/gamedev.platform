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
    return HS:call(callback, "dal.execute", dsName, sql, params, executeOpts)
end
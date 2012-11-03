-- bma/dal/host/Service.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.Core")
require("bma.lang.ext.Json")

local Class = class.define("bma.dal.host.Service", {bma.dal.BaseService})

function Class.install()
	local o = Class.new()
	class.setInstance("bma.dal.Service", o)
end

function Class:ctor()
	
end

-- function execute(callStack, dsName:string, sql:string, params:map, opts:map)
-- {records:int, resultSet:list<map>}
local filter = function(result)
	if result[1] then
		result[1] = string.json(result[1])
	end
	return result
end

function Class:execute(callStack, dsName, sql, params, executeOpts)
	local HS = class.instance("bma.host.Service")
	local ps = nil
	if params then
		ps = table.json(params)
	end
	local os = nil
	if executeOpts then
		os = table.json(executeOpts)
	end
	local cs = HS:callStack(callStack)
	cs.filter = filter
    return HS:call(cs, "dal.execute",dsName, sql, ps, os)
end
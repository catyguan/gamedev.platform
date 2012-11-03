-- bma/dal/Interface.lua

--[[
class bma.dal.Service
{
	function execute(callStack, dsName:string, sql:string, params:map or list, opts:map): {records:int, resultSet:list<map>}
}
]] 
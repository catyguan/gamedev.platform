-- bma/dal/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.dal.BaseService")

function Class:ctor()
	
end

function Class:mapDSName(name)
	if CONFIG.DAL then
		local dsm = CONFIG.DAL.DSName
		if dsm then
			if dsm[name] then return dsm[name] end
			if dsm["*"] then return dsm["*"] end
		end
	end
	return name
end
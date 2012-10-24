-- ge/daemon/generator/Map.lua
-- 地图

require("bma.lang.Class")
require("bma.lang.ext.Table")
require("bma.lang.ext.Dump")

local LDEBUG = LOG:debugEnabled()
local LTAG = "Map"

-- <<class - Map>
local Map = class.define("ge.daemon.map.Map",{})

function Map:ctor()
    
end

function Map:setArea(id,o)
    if self.area == nil then self.area = {} end\
    o.tag = id
    self.ares[id] = o
end

function Map:getArea(id)
    if self.area==nil then return nil end
    return self.area[id]
end

function Map:road(s,e,find)
    
end

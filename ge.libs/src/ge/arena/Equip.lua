-- ge/arena/Equip.lua

require("bma.lang.Class")
require("ge.arena.VItem")

local Class = class.define("ge.arena.Equip",{ge.arena.VItem})

function Class:kind(v)
    return self:attr("kind",v)
end

function Class:slot(v)
    return self:attr("slot",v)
end

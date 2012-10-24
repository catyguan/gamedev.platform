-- ge/arena/CharClass.lua

require("bma.lang.Class")
require("ge.arena.VItem")

local Class = class.define("ge.arena.CharClass", {ge.arena.VItem})

function Class:charLevelup(ch,lvl,old)    
    local l = ch:getCharBasePropNameList()
    for _,n in ipairs(l) do
        self:charPropLevelup(ch,lvl,old,name)
    end
end

function Class:charPropLevelup(ch,lvl,old,name)
    local v = self:levelPop(lvl,name)
    if v~=nil then
        local ov = self:levelProp(old,name)
        local diff = v-ov
        if diff>0 then
            ch:addCharProp(name, diff)
        end
    end    
end

function Class:defLevelProp(name,p2)
    return self:attr("levelup_"..name, p2)
end

function Class:levelProp(level, name)
    local rate = self:attr("levelup_"..name)
    if rate==nil then
        return 0
    end
    return math.floor(level*rate)
end

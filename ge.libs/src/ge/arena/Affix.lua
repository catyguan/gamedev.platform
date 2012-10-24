-- ge/arena/Affix.lua

require("bma.lang.Class")
require("bma.lang.ext.String")
require("ge.arena.Character")

-- << class - Affix >>
local Affix = class.define("ge.arena.Affix",{ge.arena.CharComponent})

Affix.ALIAS = {}

Affix.newAffix = function(data)
    local k = data.type
    local c
    if k~=nil then 
        c = Affix.ALIAS[k]
        if c==nil then 
            local f = Affix.ALIAS.finder
            if f~=nil and type(f)=="function" then
                c = f(k)
            else
                c = "arena.affix.Affix"..k:caps()
            end                
        end
    end
    
    if c==nil then
        c = "ge.arena.affix.AffixCommonProp"
    end
    local o = class.newInstance(c)
    for k,v in pairs(data) do
        if k~="type" then
            o[k] = v
        end
    end
    return o
end

function Affix:attachCharacter(ch)
    self:bindCharacter(ch, 1)
end

function Affix:detachCharacter(ch)
    self:bindCharacter(ch, -1)
end

-- << class - AffixCommonProp >>
local AffixCommonProp = class.define("ge.arena.affix.AffixCommonProp",{ge.arena.Affix})

function AffixCommonProp:createCommonProp(name,v)
    self.name = name
    self.value = v
end

function AffixCommonProp:bindCharacter(ch, k)
    ch:addCharProp(self.name, k*self.value)
end
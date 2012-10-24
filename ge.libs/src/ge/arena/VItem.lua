-- ge/arena/VItem.lua

require("bma.lang.Class")

require("ge.daemon.Matrix")
require("ge.arena.Character")
require("ge.arena.Affix")

local Class = class.define("ge.arena.VItem",{ge.arena.CharComponent, bma.lang.StdObject, ge.daemon.MatrixObject})

function Class:me(v)
    if v~=nil then self._me = v end
    return self._me
end

function Class:id(v)
    return self:attr("id",v)
end

function Class:affixList(s)
    local l = self:attr("affix")
    if l==nil and s then l = self:attr("affix",{}) end
    return l
end

function Class:newAffix(data)
    local ob = ge.arena.Affix.newAffix(data)
    table.insert(self:affixList(true),ob)
end

function Class:attachCharacter(ch)
    self._me = ch
    local l = self:affixList()
    if l~=nil then
        for _,o in ipairs(l) do
            o:attachCharacter(ch)
        end
    end
end

function Class:detachCharacter(ch)
    self._me = nil
    local l = self:affixList()
    if l~=nil then
        for _,o in ipairs(l) do
            o:detachCharacter(ch)
        end
    end
end

function Class:charLevelup(ch,lvl,old)
    local l = self:affixList()
    if l~=nil then
        for _,i in ipairs(l) do
            l:charLevelup(ch,lvl,old)
        end
    end
end
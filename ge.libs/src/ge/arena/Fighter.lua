-- ge/arena/Fighter.lua

require("bma.lang.Class")
require("ge.daemon.Thing")
require("ge.arena.Character")

local Class = class.define("ge.arena.Fighter", {ge.arena.Character})

local rebind = function(self,l)
    if l~=nil then
        for _,o in ipairs(l) do
            if o.me ~= nil then
                o:me(self)
            end
        end
    end        
end

function Class:bindChar(ob)
    local data = class.saveObject(ob)
    class.deserializeObject(self,data)
    
    -- rebind
    local cls = self:charClass()
    if cls~=nil then cls:me(self) end

    rebind(self,self:equipList())
    rebind(self,self:skillList())
    rebind(self,self:featList())
    rebind(self,self:effectList())    
end

function Class:combatAI(s)
    return self:attr("combatAI", s)
end

function Class:arena()
    return self._arena
end

function Class:beginArena(arena)
    self._arena = arena
    self:beginCombat()
end

function Class:beginCombat()
    if true then return end
    
    local name = V(self:combatState())
    if name==nil then
        error("begin Combat fail,miss combatAI");
    end
    
    local ai = OBJ(name)
    if ai==nil then
        error("begin Combat fail,miss combatAI["..ai.."]");
    end
    ai:run(self)
end

function Class:enemyList()
    local ao = self:arena()    
    return ao:fighterByHostile(self:faction(),true)    
end

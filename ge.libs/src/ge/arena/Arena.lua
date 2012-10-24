-- ge/arena/Arena.lua

require("bma.lang.Class")
require("ge.daemon.Matrix")

local MatrixObject = ge.daemon.MatrixObject

local LDEBUG = LOG:debugEnabled()
local LTAG = "Arena"

-- << class - Arena >>
local Arena = class.define("ge.arena.Arena",{MatrixObject})

MatrixObject.defEvent("ArenaBegin","Arena.begin")

Arena.REL = enum(
    {"ME","ENEMY","ALLY"}
)

function Arena:ctor()
    self.teams = {}
    self.fighters = {}
end

function Arena:combatd(v)
    if v~=nil then self._combatd = v end
    return self._combatd
end

function Arena:teamList()
    return self.teams
end

function Arena:addTeam(fac, data)
    local l = self.teams
    if l[fac]~=nil then
        error("team "..fac.." exists")
    end
    local team = ge.arena.Team.new()    
    team:parent(self)    
    team.faction = fac
    team.data = data
    l[fac] = team
    return team
end

function Arena:getTeam(fac)
    local l = self.teams
    if l==nil then return nil end
    return l[fac]
end

function Arena:removeTeam(fac)
    local l = self.teams
    if l==nil then return nil end
    local o = l[fac]
    l[fac] = nil
    if o~=nil then
        o:parent(false)
    end
    return o~=nil 
end

function Arena:fighterList()
    return self.fighters
end

function Arena:addFighter(ob)
    local fac = ob:faction()    
    local tm = self:getTeam(fac)
    if tm==nil then
        error(string.format("addFighter invalid Team(%s)",tostring(fac)))
    end
    
    table.insert(tm.fighters, ob)
    ob:parent(tm)
    
    table.insert(self.fighters, ob)    
end

function Arena:removeFighter(id)
    local l = self.fighters
    for i,e in ipairs(l) do
        if e:id()==id then
            table.remove(l,i)                
            e:parent(false)
            return true
        end
    end    
    return false
end

function Arena:fighterByFaction(fac, live, r)
    local tm = self:getTeam(fac)
    if r==nil then r = {} end
    if tm~=nil then
        for _, o in ipairs(tm.fighters) do
            if live==nil or not o:isDie() then
                table.insert(r, o)
            end
        end
    end
    return r    
end

function Arena:fighterByHostile(fac1, live)
    local tm = self:getTeam(fac1)
    local l = tm:hostile()
    local r = {}
    for _, fac2 in ipairs(l) do
        self:fighterByFaction(fac2,live, r)        
    end
    return r
end

function Arena:fighterByAlly(fac1,live)
    local tm = self:getTeam(fac1)
    local l = tm:ally()
    local r = {}
    for _, fac2 in ipairs(l) do
        self:fighterByFaction(fac2,live, r)        
    end
    return r
end

function Arena:beginArena()
    LOG:debug(LTAG,"begin Arena [%s]", self:id())
    local ev = {
        type = Arena.EVENT.ArenaBegin,
    }
    self:fireEvent(ev)
    
    local l = self:fighterList()
    for _,f in ipairs(l) do
        f:beginArena(self)
    end    
end

function Arena:endArena()
    
end


-- << class - Team >>
local Team = class.define("ge.arena.Team",{ge.daemon.MatrixObject})

function Team:ctor()
    self.relations = {}
    self.fighters = {}
end

function Team:relationList()
    return self.relations
end

function Team:fighterList()
    return self.fighters
end

function Team:rel(fac2)    
    return self.relations[fac]
end

function Team:setRel(fac2,ref)    
    if type(fac2)~="table" then
        fac2 = {fac2}
    end
    for _,n in ipairs(fac2) do
        self.relations[n] = ref
    end
end

function Team:setHostile(fac2)
    self:setRel(fac2,Arena.REL.ENEMY)
end

function Team:setAlly(fac2)
    self:setRel(fac2,Arena.REL.ALLY)
end

function Team:isHostile(fac)
    return self.reloations[fac]==Class.REL.ENEMY
end

function Team:hostile()
    local r = {}
    local rt = Arena.REL.ENEMY
    for k,e in pairs(self.relations) do
        if e==rt then
            table.insert(r, k)
        end
    end
    return r
end

function Team:isAlly(fac)
    return self.reloations[fac]==Class.REL.ALLY
end

function Team:ally()
    local r = {}
    local rt = Class.REL.ALLY
    for k,e in ipairs(tm.relation) do
        if e==rt then
            table.insert(r, k)
        end
    end
    return r
end


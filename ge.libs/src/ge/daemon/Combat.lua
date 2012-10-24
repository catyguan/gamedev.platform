-- ge/daemon/Combat.lua
-- 定义战斗规则和算法
-- Damage Increase,Attack Per Second,Critical Hit Chance,Critical Hit Damage
-- Block Amount,Block Chance,Dodge Chance,Damange Reduction,Cold/xxxx Resistance,CC Reduction（控制）,Missile Damage Reduction,Melee Damage Reduction
-- Max Lift,Total % Lift,Life Per Second,Lift Steal,Lift Per Kill,Lift Per Hit
-- Strength, Dexterity, Intelligence, Vitality

require("bma.lang.Class")
require("bma.lang.ext.Dump")

local LDEBUG = true and LOG:debugEnabled()
local LTAG = "Combat"

-- << class - Combat >>
local Combat = class.define("ge.daemon.Combat",{})

function Combat:ctor()
    self.random = math.random
    self.LEVEL_DEF = {}
end

function Combat:initDamage(data)
    self.DAMAGE = enum(data)
    self.DSIZE = self.DAMAGE.size() 
end

function Combat:alloc(v,t)
    local r = {}
    for i=1,self.DSIZE do
        if t then v = {} end
        table.insert(r,v)
    end
    return r
end

function Combat:levelDef(level)
    return self.LEVEL_DEF[level]
end

function Combat:levelName(group,lvl)
    return group..lvl
end

function Combat:hasLevelDef(group,lvl)
    local level = self:levelName(group,lvl)
    return self.LEVEL_DEF[level] ~= nil
end

function Combat:initLevelDef(group,lvl,data)
    local level = self:levelName(group,lvl)
    local d = self:defaultLevelDef(group,lvl)
    if data~= nil then
        for k,v in pairs(data) do
            d[k] = V(d[k],0)*v
        end
    end
    self.LEVEL_DEF[level] = d    
end

--[[
dodge,parry,critical 每1%所需要数值
armor,resist = x(x+VALUE)
self.LEVEL_DEF = {
    L1 = {dodge=5, parry=5, critical=5, armor=100, resist=100},
}
]]
function Combat:defaultLevelDef(group,lvl)
    return {
        dodge = 10, 
        parry = 10, 
        critical = 10, 
        armor = 50, 
        resist = 50, 
    }
end

--[[
attacker {    
    x = int,
    y = int,
    range = int,
    heal = {min:int,max:int}, 
    critical = {{critPoint:int,critHeal:%}...}
}
defender {
    x = int,
    y = int,
    level = string,
    modifier = {%}
}
]]
function Combat:heal(attacker,defender,env)   
    if LDEBUG then
        LOG:debug(LTAG,"heal()")
        LOG:debug(LTAG,"attacker[%s]",string.dump(attacker)) 
        LOG:debug(LTAG,"defender[%s]",string.dump(defender))
    end
    
    local healRange = attacker.range
    if healRange~=nil then
        local ax = attacker.x
        local ay = attacker.y
        local dx = defender.x
        local dy = defender.y
        if ax==nil or ay==nil or dx==nil or dy==nil then
            error(string.format("pos of attacker(%s,%s),defender(%s,%s) invalid",
                tostring(ax),tostring(ay),tostring(dx),tostring(dy)))
        end
        local dis = self:distance(ax,ay,dx,dy)
        if healRange<dis then
            if LDEBUG then
                LOG:debug(LTAG,"range fail [%f vs %f]",healRange,dis)
            end
            return {hit=false,beyond=true,heal=0}
        else
            if LDEBUG then
                LOG:debug(LTAG,"range done [%f vs %f]",healRange,dis)
            end
        end
    end
    
    local h = attacker.heal
    if h == nil then
        error("attacker heal is nil")
    end
    local resp = {}
    local r = 0
    
    local heal = self.random(h[1], h[2])
    local org = heal    
    if heal>0 then
        if defender.modifier~=nil then
            for j=1,#defender.modifier do
                heal = heal*defender.modifier[j]
            end
        end
        if LDEBUG then
            LOG:debug(LTAG,"heal = %d/%d/%d-%d",heal,org,h[1],h[2])
        end
        r = heal
    end
    
    local critList = attacker.critical    
    if heal>0 and critList~=nil then
        for _, e in ipairs(critList) do
            local crit = e[1]
            local critHeal = e[2]
            
            local ddef = self:levelDef(defender.level)
            local p = crit/ddef.critical*100
            local c = self.random(0,10000)
            if c < p then
                -- critical                
                if LDEBUG then
                    LOG:debug(LTAG,"critical(%f) heal [%d vs %d]",critHeal,p,c)            
                end
                r = r*(1 + critHeal)
                resp.critical = true
            else
                if LDEBUG then
                    LOG:debug(LTAG,"critical miss [%d vs %d]",p,c)
                end
            end
        end     
    end
    r = math.floor(r)
    
    if LDEBUG then
        LOG:debug(LTAG,"heal => %d",r)       
    end
    resp.hit = true
    resp.heal = r
    return resp
end

function Combat:distance(x1,y1,x2,y2)
    if y1==y2 then return math.abs(x1-x2) end
    if x1==x2 then return math.abs(y1-y2) end
    local l1 = math.abs(x1-x2)
    local l2 = math.abs(y1-y2)
    return math.sqrt(l1*l1+l2*l2)
end

--[[
attacker {
    level = int,
    x = int,
    y = int,
    range = int,
    hit = int,
    pierce = alloc(int...),
    damage = alloc({min:int,max:int}),
    critical = {{critPoint:int,critDamage:%}...}
}
defender {
    level = int,
    x = int,
    y = int,
    resist = int,
    dodge = {int...},
    parry = {{parryPoint:int,parryReduct:%}},
    armor = alloc(int),
    modifier = {%},
}
]]
function Combat:hit(attacker,defender,env)   
    if LDEBUG then
        LOG:debug(LTAG,"hit()")
        LOG:debug(LTAG,"attacker[%s]",string.dump(attacker)) 
        LOG:debug(LTAG,"defender[%s]",string.dump(defender))
    end
    
    local hitRange = attacker.range
    if hitRange~=nil then
        local ax = attacker.x
        local ay = attacker.y
        local dx = defender.x
        local dy = defender.y
        if ax==nil or ay==nil or dx==nil or dy==nil then
            error(string.format("pos of attacker(%s,%s),defender(%s,%s) invalid",
                tostring(ax),tostring(ay),tostring(dx),tostring(dy)))
        end
        local dis = self:distance(ax,ay,dx,dy)
        if hitRange<dis then
            if LDEBUG then
                LOG:debug(LTAG,"range fail [%f vs %f]",hitRange,dis)
            end
            return {hit=false,beyond=true,damange=0}
        else
            if LDEBUG then
                LOG:debug(LTAG,"range done [%f vs %f]",hitRange,dis)
            end
        end
    end
    
    -- do some(Hit,Dodge) chance check    
    if attacker.level == nil then
        error("attacker level is nil")
    end
    local adef = self:levelDef(attacker.level)
    if adef == nil then
        error("attacker level["..tostring(attacker.level).."] is invalid")
    end
    local ddef = self:levelDef(defender.level)
    if ddef == nil then
        error("defender level["..tostring(defender.level).."] is invalid")
    end
    
    local hit = V(attacker.hit,0)
    -- dodge?
    local dodgeList = defender.dodge
    if dodgeList ~= nil then
        for _,e in ipairs(dodgeList) do
            local dodge = e
            dodge = dodge - hit
            if dodge > 0 then
                local p = dodge/adef.dodge*100
                local c = self.random(0,10000)
                if c < p then
                    -- dodge it
                    if LDEBUG then
                        LOG:debug(LTAG,"dodge done [%d vs %d(%d/%d)]",c,p,dodge,e)
                    end
                    return {hit=false,dodge=true,damage=0}
                else
                    if LDEBUG then
                        LOG:debug(LTAG,"dodge fail [%d vs %d(%d/%d)]",c,p,dodge,e)
                    end
                end
            end
        end
    end
    
    local resp = {}
    local modifier = {}
    if defender.modifier ~= nil then
        for _,v in ipairs(defender.modifier) do
            table.insert(modifier,v)
        end
    end
     
    -- parry?
    local parryList = defender.parry
    if parryList ~=nil then
        for _,e in ipairs(parryList) do
            local parry = V(e[1], 0)
            -- parry = parry - hit
            if parry > 0 then
                local p = parry/adef.parry*100
                local c = self.random(0,10000)
                if c < p then
                    -- parry it
                    local pr = V(e[2], 0)
                    if LDEBUG then
                        LOG:debug(LTAG,"parry(%f) done [%d vs %d(%d/%d)]",pr,c,p,parry,e[1])
                    end
                    if pr > 0 then
                        resp.parry = true
                        table.insert(modifier,1-pr)                    
                    end
                else
                    if LDEBUG then
                        LOG:debug(LTAG,"parry fail [%d vs %d(%d/%d)]",c,p,parry,e[1])
                    end
                end
            end
        end
    end    
    
    -- resist reduct
    local resistReduct = 0
    local resist = V(defender.resist, 0)    
    if resist ~= 0 then        
        resistReduct = resist/(resist+adef.resist)
    end
    
    -- damage    
    if attacker.damage == nil then
        error("attacker damage is nil")
    end
    local critDam = 1
    local critList = attacker.critical
    if critList~=nil then
        for _,e in ipairs(critList) do
            local crit = V(e[1],0)
            if crit>0 then
                local p = crit/ddef.critical*100
                local c = self.random(0,10000)
                if c < p then
                    -- critical it
                    cdam = V(e[2], 0)
                    if LDEBUG then
                        LOG:debug(LTAG,"critical(%f) hit [%d vs %d(%d)]",cdam,p,c,crit)
                    end
                    if cdam>0 then
                        resp.critical = true
                        critDam = critDam * (1+cdam)
                    end
                else
                    if LDEBUG then
                        LOG:debug(LTAG,"critical miss [%d vs %d(%d)]",p,c,crit)
                    end
                end
            end
        end
    end
    
    local r = 0
    resp.damageInfo = {}    
    for i=1,self.DSIZE do        
        local dm = attacker.damage[i]
        local dam = 0        
        local odam = 0
        if dm ~= nil then 
            odam = self.random(dm[1],dm[2])
            dam = odam * critDam
        end
        
        local org = dam
        if dam>0 then
            if resistReduct > 0 then 
                dam = dam*(1-resistReduct)
            end
                        
            local pierce = 0
            if attacker.pierce~=nil then
                pierce = V(attacker.pierce[i],0)
            end
            local armor = 0
            if defender.armor~= nil then            
                armor = V(defender.armor[i], 0)
            end
            
            local ar = armor - pierce
            local arModifier = 0
            if ar > 0 then
                arModifier = ar/(math.abs(ar)+adef.armor)
                dam = dam*(1-arModifier)
            end
            
             -- modifier
            for _,v in ipairs(modifier) do                    
                dam = dam*v
            end            
            if LDEBUG then
                LOG:debug(LTAG,"damage[%s] = %d/%d/%d(%d-%d) R:%.2f A:%.2f M:%s",
                        self.DAMAGE.valueOf(i),
                        dam,org,odam,dm[1],dm[2],
                        resistReduct, arModifier, table.concat(modifier,","))
            end
            dam = math.floor(dam) 
            r = r + dam
        end   
        table.insert(resp.damageInfo, dam)
    end    
    if LDEBUG then
        LOG:debug(LTAG,"damage => %d",r)       
    end
    resp.hit = true
    resp.damage = r
    return resp
end

function Combat:hitTest(level,type,v)
    local def = self:levelDef(level)
    if type=="dodge" then
        return v/def.dodge
    end
    if type=="parry" then
        return v/def.parry
    end
    if type=="critical" then
        return v/def.critical
    end
    if type=="armor" then
        return v/(v+def.armor)
    end
    if type=="resist" then
        return v/(v+def.resist)
    end
    return -1
end

require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Combat")

local C = class.define("test.Combat",{ge.daemon.Combat})
function C:ctor()
    -- DAMAGE enums
    self:initDamage({"NORMAL", "SPECIAL"})
    for i=1,5 do
        self:initLevelDef("L",i)
    end    
end
function C:defaultLevelDef(group,lvl)
    return {
        dodge=5*lvl, parry=5*lvl, critical=5*lvl, armor=100*lvl, resist=100*lvl
    }
end

if false then
    -- heal
    local combatd  = C.new()  
    local attacker = {        
        range=10,
        x=0,
        y=0,
        heal = {800,1000},
        critical = {{50*5,0.1},{50,0.5}}            
    }
    local defender = {
        x=10,
        y=0,
        level = "L1",
        modifier = {1.1}
    }
    local r = combatd:heal(attacker,defender)
    print("heal",string.dump(r))
end

if true then
    local combatd  = C.new()  
    local attacker = {
        level = "L1",
        x = 10,
        y = 3,
        range = 10,
        hit = 5,
        pierce = {80,80},
        damage = {{800,1000},{400,600}},
        critical = {{50,0.5},{50*5,0.1}},
    }
    local defender = {
        level = "L1",
        x = 1,
        y = 1,
        resist = 150,
        dodge = {58,108},
        parry = {{30,0.5},{30*5,0.1}},        
        armor = {300,50},
        modifier = {0.9}
    }
    local r = combatd:hit(attacker,defender)
    print("hit",string.dump(r))
end

if false then
    local lv = 50
    local st = 10
    for i=1,1000,100 do
        print(i,i/(i+st*lv))
    end
    
    local i = 500
    local v = 100
    for v=100,1000,100 do
        print(v,i/(i+v))
    end
end



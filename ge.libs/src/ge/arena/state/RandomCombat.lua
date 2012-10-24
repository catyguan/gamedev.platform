-- ge/arena/state/RandomCombat.lua

require("bma.lang.Class")
require("ge.daemon.Matrix")
require("ge.daemon.Command")
require("ge.daemon.State")
require("ge.daemon.Thing")

-- << class/state - CombatState >>
local Class  = class.define("ge.arena.state.RandomCombat", {ge.daemon.state.ObjectState})

function Class:activeCommand()
    local cmd = self:tempv("cmd")
    if cmd==nil then
        cmd = self:performCommand()
        cmd:tag(NEWTAG())
        self:tempv("cmd",cmd)
    end
    MT():delayCommand(1, cmd)
    return cmd
end

function Class:process(ret)
    local nt = 1000
    local me = self:me()
    
    local elist = me:enemyList()     
    if #elist>0 then
        local sel = MT():random(1,#elist)
    end
    
    ret.resp = true
    ret.nextTime = nt
    return true
end
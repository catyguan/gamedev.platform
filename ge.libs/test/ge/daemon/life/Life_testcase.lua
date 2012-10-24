-- 模拟人生测试用例
require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")
require("bma.lang.ext.Table")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Thing")
testcase.include("ge.daemon.Command")
testcase.include("ge.daemon.Living")
testcase.include("ge.daemon.HFSM")

local Matrix = ge.daemon.Matrix
local Living = ge.daemon.Living
local HFSM = ge.daemon.HFSM
local GroupState = ge.daemon.state.GroupState
local BaseState = ge.daemon.state.BaseState

-- << Class - State >>
local Idle = class.define("test.state.Idle",{BaseState})
function Idle:evaluate(ctx)
    return BaseState.EVAL.SELECT, 0
end
function Idle:process(ctx)
    LOG:debug("Idle","....")
    return false
end

testcase.include("ge.daemon.life.Sleep")

-- << Class - NPC >>
local ME = class.define("test.me", {Living})
function ME:ctor()
end
function ME:create()
    -- 存款
    self:prop("money", 100)
    -- 体力
    self:prop("energy",80)
    self:prop("maxEnergy",100)
    -- 能力
    self:prop("work", 10)
    -- 智力
    self:prop("iq", 10)
    -- 职业
    self:prop("job","")
    -- 职业经验
    self:prop("jobExp",0)
    
    -- HFSM
    local ai = HFSM.new()    
    ai:add("idle",Idle.new())    
    ai:add("sleep",test.state.Sleep.new())
    
    self.ai = ai
    
    self:startAI()
end
function ME:changeEnergy(v,why)
    local e = self:prop("energy")
    local ne = e+v
    if ne<0 then ne = 0 end
    if ne>self:prop("maxEnergy") then ne = self:prop("maxEnergy") end
    if ne == e then return end
    self:changeProp("energy", ne, why)
end

--<< RUN >>
local init = function(m)    
    m.config.Living.aiTime = 3600
    
    m.timeLabelFun = function(t,st)
        return os.date("%Y-%m-%d %H:%M:%S",t+st)
    end
    
    local lis = function(ev)
        print("\nevent",string.dump(ev),"\n")
    end    
    m:listen(lis)
    
end

local creator = function(m)
    -- create runtime objects    
    me = ME.new()
    m:enterObject("me",me)    
    me:create()
end

if true then
    m  = Matrix.new()
    m:setup(os.time(), 0, init,creator)
    
    -- start
    m:begin()   
        
    local rtime = 3600*13
    m:process(rtime)
    
    print("now", m:dumpTime())
    print("commands", "\n"..m:dumpCommand())
    
end

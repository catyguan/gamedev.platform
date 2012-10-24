require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("ge.daemon.HFSM")
-- testcase.include("ge.daemon.state.Precondition")

local BaseState = ge.daemon.state.BaseState
local GroupState = ge.daemon.state.GroupState
local HFSM = ge.daemon.HFSM

local S1 = class.define("test.S1", {BaseState})
function S1:ctor()
    self.flag = "S1"
end
function S1:evaluate(rt, ctx)
    return BaseState.EVAL.SELECT, math.random(100)
end
function S1:process()
    print("in S1")
end

if true then
    local sm = HFSM.new()    
    
    local gs1 = GroupState.new()
    local bs1 = ge.daemon.state.WrapState.new()
    bs1:bind(S1.new())
    gs1:add("b1",bs1)
    local bs2 = BaseState.new()
    gs1:add("b2",bs2)
    
    sm:add("a1",gs1)
        
    sm:initHFSM(nil)
    
    sm:process({})   
    
    sm:pushState({"a1","b2"})        
    sm:process({})
    
    sm:popState()
    sm:process({})

    print("HFSM")
    var_dump(sm)
    print("RUNTIME")
    var_dump(rt)
    
end

require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("ge.daemon.State")

local BaseState = ge.daemon.state.BaseState

local S1 = class.define("test.S1", {BaseState})
function S1:ctor(w)
    self:name(w)
end
function S1:process()
    print("in",self:name())
end

local visitor = function(st, active)
    if active then 
        print("visit "..V(st:name(),"?"))
    end
    return true
end

if false then
    -- stack state
    local sm = ge.daemon.state.StackState.new()    
    sm:name("STACK")
    
    sm:walk(visitor)
    
    sm:pushState(S1.new("s1"))        
    sm:pushState(S1.new("s2"))        
    sm:pushState(S1.new("s3"))        
    sm:pushState(S1.new("s4"))        
    sm:walk(visitor)
    
    sm:popState()
    sm:walk(visitor)

    print("HFSM")
    var_dump(sm)
end

if false then
    -- group
    local sm = ge.daemon.state.GroupState.new()   
    sm:name("GROUP")
    
    sm:addState(S1.new("s1"))        
    sm:addState(S1.new("s2"),"my2")        
    sm:addState(S1.new("s3"))        
    sm:addState(S1.new("s4"))        

    sm:walk(visitor)
    
    print("HFSM")
    var_dump(sm)
end

if false then
    -- mul group>
    local sm = ge.daemon.state.GroupState.new()    
    sm:name("ROOT")
    
    local sm2 = ge.daemon.state.GroupState.new()    
    sm2:name("L1")
    sm2:addState(S1.new("s1"))        
    sm2:addState(S1.new("s2"))        
    sm2:addState(S1.new("s3"))        
    sm2:addState(S1.new("s4"))        
    
    sm:addState(sm2)

    sm:walk(visitor)
    
    print("HFSM")
    var_dump(sm)
end

if true then
    -- mix
    local sm = ge.daemon.state.MixState.new()    
    sm:name("MIX")
    
    sm:pushState("P1", S1.new("s1"))        
    sm:pushState("P1", S1.new("s2"))        
    sm:pushState("P2", S1.new("s3"))        
    sm:pushState("P2", S1.new("s4"))        
        
    sm:walk(visitor)
    
    sm:popState("P2")        
    sm:popState("P3")
    
    sm:walk(visitor)
    
    print("HFSM")
    var_dump(sm)
end

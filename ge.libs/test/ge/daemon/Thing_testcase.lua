require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Command")
testcase.include("ge.daemon.State")
testcase.include("ge.daemon.Thing")

local Matrix = ge.daemon.Matrix
local Thing = ge.daemon.Thing

local init1 = function(m)
    m.timeLabelFun = Matrix.TLF_SECOND
    
    local lis = function(ev)
        print("event",string.dump(ev))
    end    
    m:listen(lis)
end

local m  = Matrix.new()
init1(m)
m:setup(0,0)

if false then
    -- property event
    local th = Thing.new()
    th:id("o1")
    m:addObject(th)
        
    th:changeProp("life",9,"for test")
    
end

if false then   
    -- object state & lock 
    local S1 = class.define("test.state.S1",{ge.daemon.state.ObjectState})
    function S1:ctor()
        self:addBusy("see")
    end
    function S1:process(...)
        print("in s1")
    end
    
    local th = Thing.new()
    th:id("o1")
    m:addObject(th)
    
    local s1 = S1.new()
    s1:name("s1")
    th:pushState("test", s1)    
    print("can see?", not th:isBusy("see"))
    print("can walk?", not th:isBusy("walk"))
    
    th:popState("test")  
    print("can see again?", not th:isBusy("see"))
    
end

if true then
    -- state with activeCommand
    local S1 = class.define("test.state.S1",{ge.daemon.state.ObjectState})
    function S1:ctor()
        self.busy = "see"
    end
    function S1:activeCommand()        
        local cmd = ge.daemon.command.RepeatCommand.new()
        cmd.execute = function()
            print("in s1")    
        end
        cmd:createRepeat(1000)
        cmd:runNow()
        return cmd
    end
    
    -- Thing perform
    local th = ge.daemon.Thing.new()
    th:id("o1")
    m:addObject(th)
        
    m:begin()
    
    local s1 = S1.new()
    s1:name("s1")
    th:pushState("test",s1)
    
    m:process(2000)
    
    th:popState("test")
        
    print("now", m:dumpTime())
    print("commands", "\n"..m:dumpCommand())
    
end

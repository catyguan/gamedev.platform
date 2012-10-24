require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")
require("bma.lang.ext.Table")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Command")
testcase.include("ge.daemon.State")
testcase.include("ge.daemon.Thing")
testcase.include("ge.daemon.life.Human")

local Matrix = ge.daemon.Matrix
local Human = test.Human

--<< RUN >>
local init = function(m)    
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
    me = Human.new()
    m:enterObject("me",me)    
    me:create()
end

m  = Matrix.new()
m:setup(os.time(), 0, init, creator)
m:begin()   

local me = m:obj("me")

if false then    
    me:sleep(3600*4)
    
    local rtime = 3600*9
    m:process(rtime)
end

if true then
    me:sleep(3600*4)
    
    local rtime = 3600*3
    m:process(rtime)
    
    me:getState("main"):pushState(ge.daemon.state.BaseState.new())
    me:getState("main"):popState()
    m:process(rtime)
end

print("now", m:dumpTime())
print("commands", "\n"..m:dumpCommand())

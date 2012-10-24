require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Command")

local M = ge.daemon.Matrix

local hello = class.define("test.hello",{ge.daemon.MatrixCommand})
hello.create = function(word,ret)
    local r = hello.new()
    r.word = word
    r.ret = ret
    return r
end
function hello:execute()
    print("hello "..V(self.word,"???"))
    return self.ret
end

local m  = M.new()
m.timeLabelFun = M.TLF_SECOND
m:setup(0,0)

if false then
    -- base
    local cls = class.define("test.o1",{ge.daemon.MatrixObject})
    function cls:hello(world)
        print("hello "..world)
    end
    local o1 = cls.new()
    
    m:runCommand(1100,o1:callCommand("hello","kitty"))
end

if false then
    -- sequence
    local cmd0 = hello.create("0")
    local cmd1 = hello.create("1")
    local cmd2 = hello.create("2")
    
    local cmd = ge.daemon.command.SequenceCommand.new()
    cmd:addCommand("0",cmd0)
    cmd:addCommand("1",cmd1)
    cmd:addCommand("2",cmd2)
    m:nowCommand(cmd)    
end


if true then
    -- repeat
    local cmd = ge.daemon.command.RepeatCommand.new()    
    cmd.execute = function()
        print("hello world")
    end
    
    cmd:createRepeat(500,2)
    m:nowCommand(cmd)
end

if false then
    local clsDuration = class.define("test.Duration",{ge.daemon.command.DurationCommand})
    function clsDuration:execute(tick,last)
        print("Duration",tick,last)
        return true
    end
    
    
    local cmd = clsDuration.new()
    cmd:createDuration(100,1000,330)
    cmd:run()    
    
end

m:begin()
m:process(2000)    

print("now", m:dumpTime())
print("Commands", "\n"..m:dumpCommand())
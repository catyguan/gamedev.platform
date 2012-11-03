require("bma.test.bootstrap")
require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("bma.matrix.Matrix")

local M =  bma.matrix.Matrix

local hello = class.define("test.hello",{bma.matrix.MatrixCommand})
hello.create = function(word)
    local r = hello.new()
    r.word = word
    return r
end
function hello:execute()
    print("hello "..V(self.word,"???"))
end

local m  = M.new()
m.timeLabelFun = M.TLF_SECOND
m:setup(0,0)    

m:begin()

if false then
    -- object & oid
    local o = {}
    function o:id()
        return "abc"
    end
    function o:matrixChild(oid, idx)
        return "hi "..oid[idx]
    end
    
    m:addObject(o)
    print(m:obj("abc"))
end

if false then
    local a1 = class.define("test.a1",{bma.matrix.MatrixCommand})    
    function a1:execute()
        print("a1")
    end
    
    m:runCommand(500,hello.create("world500"))
    m:runCommand(100,hello.create("world100"))
    m:runCommand(1100,a1.new())
    m:runCommand(2001,hello.create("world2001"))
    
    m:process(2000)    
    print("here")
    m:delayCommand(500,hello.create("new world500"))
    
    print("now", m:dumpTime())
    print("Commands", "\n"..m:dumpCommand())
    
end

if false then
    -- event
    local cls = class.define("test.o",{bma.matrix.MatrixObject})
    
    local f = class.define("test.f",{bma.matrix.MatrixCommand})
    function f:ctor(o)
        self.target = o
    end
    function f:execute()        
        V(self.target,MT()):fireEvent({type="test",a=1,b=2})
    end
    
    local lis = function(ev)
        print("event",string.dump(ev))
    end
    local lis2 = function(ev)
        print("obj event",string.dump(ev))
    end
    
    m:listen(lis)
    
    local o1 = cls.new()
    local o2 = cls.new()
    o2:parent(o1)
    o1:listen(lis2)
    
    m:delayCommand(500,f.new())
    m:delayCommand(500,f.new(o2))
    
    m:process(1000)    
        
    print("now", m:dumpTime())
    print("Commands", "\n"..m:dumpCommand())
    
end



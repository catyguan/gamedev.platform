require("bma.test.bootstrap")
require("bma.lang.Class")
require("bma.lang.ext.Dump")

include("bma.matrix.Matrix")
include("bma.matrix.Action")

print(_G)

local M =  bma.matrix.Matrix

local hello = class.define("test.hello",{bma.matrix.Action})
hello.create = function(word)
    local r = hello.new()
    r.word = word
    return r
end
function hello:execute()
    print("hello "..V(self.word,"???"))
end

local m  = M.new()
m:initObject()
m.timeLabelFun = M.TLF_SECOND
m:setup(0,0)    

m:begin()

if false then
    -- object & oid
    local o = {}
    function o:id()
        return "abc"
    end
    
    m:addObject(o)
    print(m:obj("abc"))
end

if false then
    local a1 = class.define("test.a1",{bma.matrix.Action})    
    function a1:execute()
        print("a1")
    end
    
    m:runAction(500,hello.create("world500"))
    m:runAction(100,hello.create("world100"))
    m:runAction(1100,a1.new())
    m:runAction(2001,hello.create("world2001"))
    
    m:process(2000)    
    print("here")
    m:delayAction(500,hello.create("new world500"))
    
    print("now", m:dumpTime())
    print("Actions", "\n"..m:dumpAction())
    
end





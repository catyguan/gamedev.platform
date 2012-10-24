require("bma.lang.Class")
require("bma.lang.ext.Dump")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Command")
testcase.include("ge.daemon.generator.ObjectGenerator")

local Matrix = ge.daemon.Matrix
local OG = ge.daemon.generator.ObjectGenerator

local OGTest = class.define("test.OGTest",{OG})
function OGTest:ctor(name,c,w)
    self.tag = name
    self.cost = c
    self.weight = w
end

function OGTest:evaluate(ctx,cost)
    if cost<self.cost then
        return nil
    end
    return self.weight
end

local OGTest2 = class.define("test.OGTest2",{OGTest})

function OGTest2:generate(ctx,cost,data)
    return cost-self.cost, "hello "..self.tag
end


local m  = Matrix.new()
m:setup(0,0)

if true then
    -- base
    local g = OG.new()
    g.tag = "main"
    
    local g1 = OGTest.new("g1",10,30)
    local g11 = OGTest2.new("g11",15,100)
    local g12 = OGTest2.new("g12",20,20)
    g1:addRule(g11)   
    g1:addRule(g12)   
    
    local g2 = OGTest2.new("g2",20,20)
    local g3 = OGTest2.new("g3",30,10)
    local g4 = OGTest2.new("g4",0,"*")
    
    g:addRule(g1)    
    g:addRule(g2)    
    g:addRule(g3)    
    g:addRule(g4)    
    
    local r = {g:generate({},50)}
    print(unpack(r))
    
end


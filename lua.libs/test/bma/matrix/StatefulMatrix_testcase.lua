
require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.matrix.StatefulMatrix")

local M =  bma.matrix.StatefulMatrix

local TestM = class.define("test.Matrix",{M})
function TestM:init()
	self.timeLabelFun = M.TLF_SECOND
	self.time = 0	
end

local hello = class.define("test.hello",{bma.matrix.MatrixCommand})
hello.create = function(word)
    local r = hello.new()
    r.word = word
    return r
end
function hello:execute()
    print("hello "..V(self.word,"???"))
end

local OC1 = class.define("test.o1")
function OC1:id()
	return "abc"
end

local SS = class.instance("bma.state.Service")

if true then
    -- object & oid
    local cb = function(err, m)
    	if err then
    		print("create matrix fail",err)
    		return
    	end
    	if not m:obj("abc") then    		
	    	local o = OC1.new()		
	    	o.name = "myname"        
		    print("create abc")
	    	m:addObject(o)
	    end
    	var_dump(m:obj("abc"))    	
    end
    
    SS:get(cb,"matrix1","test.Matrix")
    
end

if false then
    -- run
    local cb = function(err, m)
    	if err then
    		print("create matrix fail",err)
    		return
    	end
    	m:begin()
    	
    	local a1 = class.define("test.a1",{bma.matrix.MatrixCommand})    
	    function a1:execute()
	        print("a1")
	    end
	    
	    m:delayCommand(500,hello.create("world500"))
	    m:delayCommand(100,hello.create("world100"))
	    m:delayCommand(1100,a1.new())
	    m:delayCommand(2001,hello.create("world2001"))
	    
	    m:process(2000)    
	    print("here")
	    m:delayCommand(500,hello.create("new world500"))
	    
	    print("now", m:dumpTime())
	    print("Commands", "\n"..m:dumpCommand())    	    	
    end
    
    SS:get(cb,"matrix1","test.Matrix")
    
end

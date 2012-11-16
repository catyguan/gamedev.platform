require("bma.lang.ext.Dump")
require("bma.persistent.PersistentEntity")

if false then
	local SI = class.forName("bma.state.mysqlhost.Service")
	SI.install()
end	

local S = class.instance("bma.persistent.Service")

local C1 = class.define("test.o1",{bma.persistent.PersistentEntity})	
function C1:initObject()
	self:prop("word", "start")
end
function C1:isObjectValid()
	return true
end	

local C2 = class.define("test.o2",{C1})

-- load/save
if false then
	local fun = function(err, o)
		print("callback",err, string.dump(o))
    	o:prop("word" ,"next")
	end
	            
    print("load")
    S:load(fun, "id1", "test.o1", false)
    
    print("save")
   	S:save(fun, "id1", false)    
end

-- subload
if false then
    local fun = function(err, o)
        print("callback",err, string.dump(o))
        if o.className=="test.o1" then
        	if not o:hasProp("o2") then
        		o:loadProp(print,"o2", "id2", "test.o2")
        	else
        		print("already has o2")
        	end 
        end        
    end
            
    print("load")
    S:load(fun, "id1", "test.o1", false)
    
end

-- cross load
if false then
    local fun = function(err, o)
        print("callback",err, string.dump(o))
        if o.className=="test.o1" then
        	if not o:hasProp("o2") then
        		local cb1 = function()
        			print("o2", o:prop("o2"))
        		end
        		o:loadProp(cb1,"o2", "id2", "test.o2")
        	else
        		print("already has o2",o:prop("o2"))
        	end 
        end        
    end
            
    print("load")
    S:load(fun, "id1", "test.o1", false)
    S:load(fun, "id3", "test.o1", false)
    
end

-- Parent/Children
if true then
	require("bma.lang.HasParent")
	require("bma.lang.HasChildren")
	
	local TreeNode = class.define("test.TreeNode",{bma.lang.HasParent, bma.lang.HasChildrenStateful, bma.persistent.PersistentEntity})
	function TreeNode:isObjectValid()
		return true
	end
	
	local NF = function()end
    local fun = function(err, o)    	
		print("callback",err, string.dump(o))
	    local ch = o:children()
	    if #ch == 0 then	    	
	       	o:loadChild(NF,o:id().."-1","test.TreeNode")
	       	o:loadChild(NF,o:id().."-2","test.TreeNode")
	       	o:loadChild(NF,o:id().."-3","test.TreeNode")
	    end
    end
            
    print("load")
    S:load(fun, "t1", "test.TreeNode", false)
    
end
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

local C2 = class.define("test.o2",{C1})

-- load/save
if false then
    print("get")
    local o = S:get("id1", "test.o1")
    var_dump(o)
    
    local fun = function(err, v)
		print("callback",err, v,string.dump(o))
		
		print("save")
		o:prop("word", "second")
		local fun2 = function(err,v)
			print("callback2",err, v)
		end
   		S:save(fun2, "id1", false)
	end
    o:restoreObject(fun)    
end

-- subload
if true then
	 print("get")
    local o = S:get("id1", "test.o1")
    var_dump(o)
    
    local fun = function(err, v)
        print("callback",err, v, string.dump(o))
        if o.className=="test.o1" then
        	if not o:hasProp("o2") then
        		o:prop("o2", S:get("id2", "test.o2"))
        	else
        		print("already has o2")
        	end 
        end        
    end
	o:restoreObject(fun)     
    
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
if false then
	require("bma.lang.HasParent")
	require("bma.lang.HasChildren")
	
	local TreeNode = class.define("test.TreeNode",{bma.lang.HasParent, bma.lang.HasChildrenStateful, bma.persistent.PersistentEntity})
	function TreeNode:ctor()
		self.iocChildren = true
	end
	function TreeNode:isObjectValid()
		return true
	end
	
	local fun2 = function(_, co)
	    var_dump(co)
	end
    local fun = function(err, o)    	
		print("callback",err, string.dump(o))		
	    local ch = o:children()
	    if #ch == 0 then	 
	    	print("create children")
	       	o:loadChild(fun2,o:id().."-1","test.TreeNode")
	       	o:loadChild(fun2,o:id().."-2","test.TreeNode")
	       	o:loadChild(fun2,o:id().."-3","test.TreeNode")
	    end
    end
            
    print("load")
    S:load(fun, "t1", "test.TreeNode", false)
    
end
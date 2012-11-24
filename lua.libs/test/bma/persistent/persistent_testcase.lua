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
if false then
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
	
	print("get")
    local o1 = S:get("id1", "test.o1")
    local o3 = S:get("id3", "test.o1")
    var_dump(o1)
    var_dump(o3)

	local fun = function(o)
	    return function(err, v)
	        print("callback",err, v, string.dump(o))
	        if o.className=="test.o1" then
	        	if not o:hasProp("o2") then
	        		o:prop("o2", S:get("id2", "test.o2"))
	        	else
	        		print("already has o2")
	        	end
	        end    
	        print("o2",topointer(o:prop("o2")))    
	    end
	end
            
    print("restore")
    o1:restoreObject(fun(o1))     
    o3:restoreObject(fun(o3))     
    
end

-- ref each other
if false then
	
	require("bma.lang.ai.Core")
	
	print("get")
    local o1 = S:get("id1", "test.o1")
    local o3 = S:get("id3", "test.o1")
    var_dump(o1)
    var_dump(o3)

	local fun = function()
		print("callback")
		o1:prop("o3", o3)
		o3:prop("o1", o1)
		
		print("after restore")
		var_dump(o1)
    	var_dump(o3)
	end
            
    print("restore")
    local g = bma.lang.ai.Group.new(fun)    
    o1:restoreObject(g:call("1"))     
    o3:restoreObject(g:call("3"))     
    g:finishAll()
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
	
	print("get")
    local t1 = S:get("t1", "test.TreeNode")
	
    local fun = function(err, v)    	
		print("callback", err, v)		
	    local ch = t1:children()
	    if #ch == 0 then	 
	    	print("create children")
	       	t1:addChild(S:get(t1:id().."-1","test.TreeNode"))
	       	t1:addChild(S:get(t1:id().."-2","test.TreeNode"))
	       	t1:addChild(S:get(t1:id().."-3","test.TreeNode"))
	    end
	    
	    var_dump(t1)
    end
            
    print("restore")
    t1:restoreObject(fun())
    
end
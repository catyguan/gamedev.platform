require("bma.lang.ext.Dump")
require("bma.persistent.PersistentEntity")

if false then
	local SI = class.forName("bma.state.mysqlhost.Service")
	SI.install()
end	

local S = class.instance("bma.persistent.Service")

-- load/save
if false then	
	local C1 = class.define("test.o1",{bma.persistent.PersistentEntity})	
	function C1:initObject()
		self:prop("word", "start")
	end
	function C1:isObjectValid()
		return true
	end	

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
if true then
	local C1 = class.define("test.o1",{bma.persistent.PersistentEntity})
	local C2 = class.define("test.o2",{C1})
	function C1:initObject()
		self:prop("word", "start")
	end
	function C1:isObjectValid()
		return true
	end	
	
    local fun = function(err, o)
        print("callback",err, string.dump(o))
        if o.className=="test.o1" then
        	if not o:hasProp("o2") then
        		o:loadProp(print,"o2", "id2", "test.o2")
        	else
        		print("aready has o2")
        	end 
        end        
    end
            
    print("load")
    S:load(fun, "id1", "test.o1", false)
    
end

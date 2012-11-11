require("bma.lang.ext.Dump")
require("bma.state.StatefulObject")

if true then
	local SI = class.forName("bma.state.mysqlhost.Service")
	SI.install()
end	

local S = class.instance("bma.state.Service")

-- load/save
if false then	
	local C1 = class.define("test.o1",{bma.state.StatefulObject})
	function C1:ctor()
		self.word = "start"
	end
	function C1:initState(state)
		self:saveState(state)
	end
	function C1:loadState(state)
		self.word = state["word"]
	end
	function C1:saveState(state)
		state["word"] = self.word
	end

    local fun = function(err, o)
        print("callback",err, string.dump(o))
        o.word = "next"
    end
            
    print("load")
    S:load(fun, "id1", "test.o1", false)
    
    print("save")
    S:save(fun, "id1", true)    
end

-- subload
if true then
	local C1 = class.define("test.o1",{bma.state.StatefulObject})
	local C2 = class.define("test.o2",{C1})
	function C1:ctor()
		self.word = "start"
	end
	function C1:initState(state)
		self:saveState(state)
	end
	function C1:loadState(state, cb)
		self.word = state["word"]
		S:load(cb, "id2", "test.o2", false)
		return "loading"
	end
	function C1:saveState(state)
		state["word"] = self.word
	end
	function C2:loadState(state)
		self.word = state["word"]		
	end

    local fun = function(err, o)
        print("callback",err, string.dump(o))
        o.word = "next"
    end
            
    print("load")
    S:load(fun, "id1", "test.o1", false)    
    
end

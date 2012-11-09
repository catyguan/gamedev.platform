-- app/demo1/Counter.lua
require("bma.state.StatefulObject")

local Class = class.define("app.demo1.Counter",{bma.state.StatefulObject})

function Class:ctor()
	self.counter = 1
end

function Class:id(v)
	if v then self._id = v end
	return self._id
end

function Class:loadState(state)
	self.counter = state["c"]	
end

function Class:saveState(state)
	state["c"] = self.counter
end

function Class:start()
	local HS = class.instance("bma.host.Service")
	local f
	f = function()
		print("counter", self.counter, os.memuse()/1024)
		
		self.counter = self.counter + 1
		self:stateModify(true)
		
		HS:setTimer(f, 5000)
	end
	HS:setTimer(f, 5000)
end
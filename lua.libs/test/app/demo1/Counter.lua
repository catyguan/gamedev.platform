-- app/demo1/Counter.lua
require("bma.state.StatefulObject")

local Class = class.define("app.demo1.Counter",{bma.state.StatefulObject})

function Class:ctor()
	self.counter = 1
end

function Class:loadState(state)
	self.counter = state["c"] + 1
	self:stateModify(true)
	
	print("counter", self.counter)
end

function Class:saveState(state)
	state["c"] = self.counter
end
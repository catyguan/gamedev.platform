-- app/demo1/Counter.lua
require("bma.persistent.PersistentEntity")

local Class = class.define("app.demo1.Counter",{bma.persistent.PersistentEntity})

function Class:initObject()
	self:prop("counter", 1)
end

function Class:start()
	local HS = class.instance("bma.host.Service")

	local f = function()
		print("counter", self._p.counter, os.memuse()/1024)
		
		self:prop("counter", self._p.counter + 1)
		
		return true
	end
	HS:setFixTimer(f, 5000)
end
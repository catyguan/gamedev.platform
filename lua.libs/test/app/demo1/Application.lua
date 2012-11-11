-- app/demo1/Application.lua
require("bma.app.StatefulApplication")
require("bma.app.AppService")

local Class = class.define("app.demo1.Application",{bma.app.StatefulApplication})

function Class:listMainObject()
	local r = {}
	r["counter"] = "app.demo1.Counter"
	r["c1"] = "app.demo1.Counter"
	r["c2"] = "app.demo1.Counter"
	return r
end

function Class:onLaunch(olist)
	LOG:info("demo1", "onLaunch")
	local o1 = olist.counter
	-- o1:start()
	
	local SS = class.instance("bma.state.Service")
	SS:schedule("counter")	
	-- SS:schedule("c1")
	-- SS:schedule("c2")
	
	local HS = class.instance("bma.host.Service")	
	HS:setTimer(function() SS:unschedule("counter") end, 60*1000*1.5) 
	
	return true
end

function Class:serviceTs()
	local s = bma.app.AppService.new()
	s.apiTapi = function(w)
		return "Hello "..w
	end
	return s
end
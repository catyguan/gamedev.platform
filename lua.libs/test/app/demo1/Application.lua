-- app/demo1/Application.lua
require("bma.app.StatefulApplication")
require("bma.app.AppService")

local Class = class.define("app.demo1.Application",{bma.app.StatefulApplication})

function Class:listMainObject()
	local r = {}
	table.insert(r,{id="counter", type="app.demo1.Counter"})
	return r
end

function Class:onLaunch(olist)
    print("demo1","onLaunch");
	LOG:info("demo1", "onLaunch")	
	return true
end

function Class:serviceTs()
	local s = bma.app.AppService.new()
	s.apiTapi = function(w)
		return "Hello "..w
	end
	return s
end
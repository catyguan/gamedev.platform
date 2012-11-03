-- app/app1/Application.lua
require("bma.app.Application")
require("bma.app.AppService")

local Class = class.define("app.demo1.Application",{bma.app.Application})

function Class:init()
    print("app1","print");
	LOG:info("app1", "init")
	LOG:info("app1", "init2")
	return true
end

function Class:close(destroy)
	LOG:info("app1", "close "..tostring(destroy))
	return true
end

function Class:serviceTs()
	local s = bma.app.AppService.new()
	s.apiTapi = function(w)
		return "Hello "..w
	end
	return s
end
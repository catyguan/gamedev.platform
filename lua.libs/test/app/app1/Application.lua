-- app/app1/Application.lua
require("bma.app.Application")
require("bma.app.AppService")

local Class = class.define("app.app1.Application",{bma.app.Application})

function Class:init()
	LOG:info("app1", "init")
	return true
end

function Class:close()
	LOG:info("app1", "close")
end

function Class:serviceTs()
	local s = bma.app.AppService.new()
	s.apiTapi = function(w)
		return "Hello "..w
	end
	return s
end
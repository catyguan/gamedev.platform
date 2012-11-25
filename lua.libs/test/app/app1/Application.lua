-- app/app1/Application.lua
require("bma.app.Application")
require("bma.app.AppScene")

local Class = class.define("app.app1.Application",{bma.app.Application})

function Class:init()
	LOG:info("app1", "init")
	
	local s = bma.app.AppScene.new()
	s.sayCommand = function(self, cb, word)
		print("say",tostring(word))
		aicall.done(cb,nil,"hello "..tostring(word))
	end
	s.forwardCommand = function(self, cb, word)
		local r = s.forward("local", "hello", "say", word)
		aicall.done(cb, nil, r)
	end
	
	self.scenes["hello"] = s
	return bma.app.Application.init(self)
end

function Class:close()
	LOG:info("app1", "close")
end

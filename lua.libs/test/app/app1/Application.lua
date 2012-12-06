-- app/app1/Application.lua
require("bma.app.Application")
require("bma.app.AppCase")

local Class = class.define("app.app1.Application",{bma.app.Application})

function Class:init()
	LOG:info("app1", "init")
	
	local s = bma.app.AppCase.new()
	s.sayMethod = function(self, cb, _, word)
		print("say",tostring(word))
		aicall.done(cb,nil,"hello "..tostring(word))
	end
	s.forwardMethod = function(self, cb, s, word)
		print("forward", tostring(word))
		local r = s.forward("test", "hello", "say", s, word)
		aicall.done(cb, nil, r)
	end
	
	self.cases["hello"] = s
	return bma.app.Application.init(self)
end

function Class:close()
	LOG:info("app1", "close")
end

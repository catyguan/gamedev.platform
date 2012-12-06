-- app/app2/Application.lua
require("bma.app.StatefulApplication")
require("bma.lang.Session")



local Class = class.define("app.app2.Application", {bma.app.StatefulApplication})

function Class:init()
	LOG:info("app2", "init")
	
	self:initAuthCase();
	 
	return bma.app.Application.init(self)
end

function Class:initAuthCase()
	require("cases.Auth")
	require("cloud.auth.mysqlhost.Service")
	local S = cloud.auth.mysqlhost.Service.new()			
	self.cases["auth"] = cases.Auth.new(S)
end

function Class:initSessionCase()
	require("scene.Session")
	self.session = bma.lang.Session.new(128, 60*1000)
	self.session:start();
		
	self.cases["session"] = scene.Session.new(self.session)
end

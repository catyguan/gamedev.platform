-- bma/app/ApplicationService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.app.ApplicationService",{})

local LDEBUG = LOG:debugEnabled()
local LTAG = "AppService"

function Class:install()	
	if not self.id then error(self.className.." service id is nil") end
	local app = class.instance("application")
	
	if app.services[self.id] then
		app.services[self.id]:uninstall()
	end
	app.services[self.id] = self
	
	if LDEBUG then
		LOG:debug(LTAG, "install service ["..self.id.."] ==> " .. self.className)
	end
end

function Class:uninstall()
	if not self.id then error("service id is nil") end
	local app = class.instance("application")
	
	app.services[self.id] = nil
	
	if LDEBUG then
		LOG:debug(LTAG, "uninstall service ["..self.id.."] ==> " .. self.className)
	end
end

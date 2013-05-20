-- bma/app/ApplicationService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.app.ApplicationService",{})

-- Class:createControl()
-- Class:createDataHelper()

function Class:install()	
	if not self.id then error("service id is nil") end
	local app = class.instance("application")
	
	if app.services[self.id] then
		app.services[self.id]:uninstall()
	end
	app.services[self.id] = self
	
	local c
	if self.createControl then
		c = self.createControl()
	end
	if c then
		app.objects[self.id] = c
	end
	
	local helper
	if self.createDataHelper then
		helper = self:createDataHelper()
	end
	if helper then		
		app.datas[self.id] = DATAHELPER(helper)
	end
end

function Class:uninstall()
	if not self.id then error("service id is nil") end
	local app = class.instance("application")
	
	app.services[self.id] = nil
	app.objects[self.id] = nil
	app.datas[self.id] = nil
end
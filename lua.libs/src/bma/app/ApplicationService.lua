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
	
	if self.createControl then
		local c = self:createControl()
		if c then
			app.objects[self.id] = c
		end	
	end	
	if self.createData then
		local data = self:createData()
		if data then		
			app.datas[self.id] = data
		end
	end	
end

function Class:uninstall()
	if not self.id then error("service id is nil") end
	local app = class.instance("application")
	
	app.services[self.id] = nil
	app.objects[self.id] = nil
	app.datas[self.id] = nil
end
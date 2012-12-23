-- bma/c2dx/app/API.lua
function API_Application_shutdown()
	if class.hasInstance("application") then
		LOG:debug("API","application shutdown")
		local o = class.instance("application")
		o:close()
		class.setInstance("application",nil)
	end
end

function API_Application_startup()
	API_Application_shutdown()
	LOG:debug("API","application startup")
	local cls = class.forName("bma.c2dx.app.Application")
	cls.startup()
end

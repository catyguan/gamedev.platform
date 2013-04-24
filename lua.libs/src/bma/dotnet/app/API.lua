-- bma/dotnet/app/API.lua
function _API_application_shutdown()
	if class.hasInstance("application") then
		LOG:debug("API","application shutdown")
		local o = class.instance("application")
		o:close()
		class.setInstance("application",nil)
	end
end

function _API_application_startup()
	_API_application_shutdown()
	LOG:debug("API","application startup")
	local cls = class.forName("bma.dotnet.app.Application")
	cls.startup()
end

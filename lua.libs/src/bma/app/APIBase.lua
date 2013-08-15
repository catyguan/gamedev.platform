-- bma/app/APIBase.lua

function _API_application_shutdown()
	if class.hasInstance("application") then
		LOG:debug("API","application shutdown")
		local o = class.instance("application")
		o:close()
		class.setInstance("application",nil)
	end
end

function base_API_application_startup(appClass)
	_API_application_shutdown()
	LOG:debug("API","application startup")
	local cls = class.forName(appClass)
	cls.startup()
end

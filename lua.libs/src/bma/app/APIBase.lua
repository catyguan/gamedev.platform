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

function _API_application_objectcall(name, ...)
	if class.hasInstance("application") then
		-- LOG:debug("API","objectcall "..tostring(name).." "..string.dump(...))
		local o = class.instance("application")
		return o:dispatchObjectCall(name,...)
	else
		error("application not start")
	end
end

function _API_application_dataget(name)
	if class.hasInstance("application") then
		local o = class.instance("application")
		return o:dispatchDataGet(name)
	else
		error("application not start")
	end
end

function _API_application_dataset(name, val)
	if class.hasInstance("application") then
		local o = class.instance("application")
		return o:dispatchDataSet(name, val)
	else
		error("application not start")
	end
end
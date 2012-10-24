if not LUA_APP_ID then
	error("undefined LUA_APP_ID")
end
if not LUA_APP_TYPE then
	error("undefined LUA_APP_TYPE")
end
local cls = class.forName("bma.app.Application")
cls.startup()


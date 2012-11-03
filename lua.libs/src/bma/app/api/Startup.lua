if not LUA_APP_ID then
	error("undefined LUA_APP_ID")
end
if not LUA_APP_TYPE then
	error("undefined LUA_APP_TYPE")
end

local boot = require("bma.bootstrap")
boot.config = "app."..LUA_APP_TYPE..".config"
boot.run()

local cls = class.forName("bma.app.Application")
cls.startup()


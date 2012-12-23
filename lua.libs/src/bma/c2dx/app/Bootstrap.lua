if not LUA_APP_TYPE then
	error("undefined LUA_APP_TYPE")
end

local boot = require("bma.bootstrap")
boot.config = "app."..LUA_APP_TYPE..".config"
boot.run()

require("bma.c2dx.app.API")


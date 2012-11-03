LUA_APP_ID = "test1"
LUA_APP_TYPE = "app1"

require("bma.app.api.Startup")

local app = class.instance("application")
print("appCall",app:appCall("ts","tapi","world"))

require("bma.app.api.Shutdown")
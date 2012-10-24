require("bma.test.bootstrap")

LUA_APP_ID = "test1"
LUA_APP_TYPE = "app1"

require("bma.app.Startup")

local app = class.instance("application")
print("appCall",app:appCall("ts","tapi","world"))

require("bma.app.Shutdown")
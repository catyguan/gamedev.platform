-- bma/lang/ext/Json.lua

local cjson = require("cjson")

string.json = cjson.decode
table.json = cjson.encode

module("lang.Json", package.seeall)

encode = cjson.encode
decode = cjson.decode

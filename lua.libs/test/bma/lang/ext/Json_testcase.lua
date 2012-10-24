---- json
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")

testcase.include("bma.lang.ext.Json")
local Json = lang.Json

local data = { true, { foo = "bar" } }
-- encode
print(Json.encode(data))
print(table.json(data))
-- decode
local dump = function(t)
    print(t,string.dump(t))
end
local s = Json.encode(data)
dump(Json.decode(s))
dump(s:json())


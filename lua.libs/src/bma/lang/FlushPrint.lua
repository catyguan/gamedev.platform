-- bma/lang/FlushPrint.lua

local org_print = print
print = function(...)
    org_print(...)
    io.stdout:flush()
end
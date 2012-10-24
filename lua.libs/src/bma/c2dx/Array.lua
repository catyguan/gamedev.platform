-- bma/c2dx/Array.lua
-- CCArray的功能扩展

module(..., package.seeall)

function foreach(a, fun)
    local i
    local c = a:count()-1
    for i=0,c do
        local o = a:objectAtIndex(i)
        fun(o)
    end
end


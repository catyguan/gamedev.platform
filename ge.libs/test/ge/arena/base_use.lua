-- ge.Arena 的基本用法

CORE.unload("bma.lang.OjectCreator")
CORE.unload("ge.Arena")
CORE.unload("arena.living.BaseLiving")

local ARENA = require("ge.Arena")

local arena = ARENA.new()

if true then
    local o = arena:createLiving({
        type="BaseLiving",        
        name="Tester",
        properties = {
            p1 = 1
        },
    })
    print("1", o:dump())
    o.name = "new"
    print("2", o:dump())    
end    
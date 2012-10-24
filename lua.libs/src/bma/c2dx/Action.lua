-- bma/c2dx/Action.lua

module(..., package.seeall)

local actionManager = CCDirector:sharedDirector():getActionManager()

function sequence(actions)
    local arr = CCArray:create(#actions)
    for i = 1, #actions do
        arr:addObject(actions[i])
    end
    return CCSequence:create(arr)
end

-- bma/c2dx/Scheduler.lua

module(..., package.seeall)

stack = {}
local sharedScheduler = CCDirector:sharedDirector():getScheduler()

local function push(handle)
    table.insert(stack,handle)
    return handle
end

function enterFrame(listener, isPaused)
    if type(isPaused) ~= "boolean" then isPaused = false end
    return push(sharedScheduler:scheduleScriptFunc(listener, 0, isPaused))
end

function firstEnterFrame(listener, isPaused)
    local handle
    handle = enterFrame(function()
        unschedule(handle)
        listener()
    end, isPaused)
    return handle
end

function schedule(listener, interval, isPaused)
    if type(isPaused) ~= "boolean" then isPaused = false end
    return push(sharedScheduler:scheduleScriptFunc(listener, interval, isPaused))
end

function unschedule(handle)
    sharedScheduler:unscheduleScriptEntry(handle)
    for i = 1, #stack do
        if stack[i] == handle then
            table.remove(stack, i)
            return
        end
    end
end
remove = unschedule

function unscheduleAll()
    for i = 1, #stack do
        sharedScheduler:unscheduleScriptEntry(stack[i])
    end
    stack = {}
end
removeAll = unscheduleAll

function performWithDelay(time, listener)
    local handle
    handle = sharedScheduler:scheduleScriptFunc(function()
        unschedule(handle)
        listener()
    end, time, false)
    return push(handle)
end
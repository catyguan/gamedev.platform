local BGTask = require("bma.lang.BGTask")

-- call & response
if true then
    local fun = nil
    fun = function(msgId,err,data)
        print(string.format("callback1 -> %d,%d,%s",msgId,err,data))    
    end
    BGTask.call(1,"hello world",fun)
    BGTask.call(1,"hello world2",fun)
    print(BGTask.tryCall(1,"hello world3",fun))
end    

if false then
    local fun = nil
    fun = function(msgId,err,data)
        print(string.format("callback1 -> %d,%d,%s",msgId,err,data))    
        -- bgtask.call(1,"hello world x",fun)
    end
    -- bgtask.call(1,"hello world",fun)
    -- bgtask.call(1,"hello world2",fun)
    print(bgtask.tryCall(1,"hello world3",fun))
end    


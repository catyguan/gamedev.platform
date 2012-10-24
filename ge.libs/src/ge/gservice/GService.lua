-- ge/gservice/GService.lua

require("bma.lang.Class")
require("bma.lang.ext.Json")

local LDEBUG = LOG:debugEnabled()
local LTAG = "GService"

-- << class - GService >>
local handlers = {}

function gservice(reqstr)
    local req = reqstr:json()
    local cmd = req.command
    local params = req.params
    local done,r,stt = pcall(function()
        if cmd=="require" then
            require(params)
            return true
        end
        
        local f = handlers[cmd]
        if f==nil then
            error(string.format("command[%s] undefined",tostring(cmd)))
        end
        return f(params)
    end)
    print(done,r,stt)
    local rep
    if done then
        if type(r)=="table" then
            rep = r
        else
            rep = { message = r }
        end
    else
        rep = {
            err = true,
            message = r,
            stackTrace = stt,
        }        
    end
    return table.json(rep)
end

GSERVICE_DEF = function(cmd,f)
    handles[cmd] = f
end

GSERVICE_ERR = function(msg)
    return { err=true, message=msg }
end
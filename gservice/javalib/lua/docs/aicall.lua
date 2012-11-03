if false then
    function aicalllTest(callId, word)
        local HS = class.instance("bma.host.Service")
        local f = function(r,word)
            HS:aiResponse(callId, word)
        end
        HS:call(f,"asyn",word)
        return true
    end
else
    require("bma.host.AICall")    
    function aicalllTest(callId, word)
        aicall("asyn",word):response(callId):run()
        return true
    end
end

function aitimeout(callId,word)
    print("timeout - ",callId,word)
    return true
end

print("aicall.lua loaded")
return true
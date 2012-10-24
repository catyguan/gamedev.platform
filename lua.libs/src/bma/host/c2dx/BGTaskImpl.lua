-- bma/host/c2dx/BGTaskImpl.lua

bg_handlers = {}

function bg_dispatcher(messageId, messageContent)    
    LOG:debug("BG", messageId .. ":" .. messageContent)
    local h = bg_handlers[messageId]
    if h ~= nil then        
        local cjson = require("cjson")
        local req = cjson.decode(messageContent)        
        local r = h(messageId,req)
        return cjson.encode(r)
    end
    error("unknow messageId - " .. messageId)
end
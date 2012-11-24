-- bma/maxtrix/Action.lua
require("bma.matrix.Matrix")

-- <<class - Action>>
local Action = class.define("bma.matrix.Action", {bma.lang.StdObject})

function Action:id(v)
    return self:attr("id",v)
end

function Action:value(p1,p2)    
    return self:runv(p1,p2)
end

function Action:isCancel()
    return V(self:value("hasCancel"),false)
end

function Action:cancel()
    self:value("hasCancel",true)
end

function Action:execute()
    return false, 0
end

function Action:handleExecute()
    self:execute()    
end

function Action:executeAction()
    if self:isCancel() then return end
    self:handleExecute()
end

function Action:runNow()
    MT():nowAction(self)
end

function Action:runAt(tm)
    MT():runAction(tm, self)
end

function Action:runDelay(tm)
    MT():delayAction(tm, self)
end
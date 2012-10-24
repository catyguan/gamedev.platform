-- bma/c2dx/DebugScene.lua
-- 场景常用调试功能

module(..., package.seeall)

function createRefreshLayer(fun, pos,img)
    local LAYER = require("bma.c2dx.Layer")
    local layer = LAYER.debugLayer(nil, true)
    
    if img == nil then img = "res/images/commons/refresh.png" end        
    if pos == nil then 
        local winSize = CCDirector:sharedDirector():getWinSize()
        print(winSize.width,winSize.height)
        pos = {32/2, winSize.height - 32/ 2} 
    end      
        
    local menuToolsItem = CCMenuItemImage:create(img, img)
    menuToolsItem:setPosition(0, 0)
    menuToolsItem:registerScriptHandler(fun)
    local menuTools = CCMenu:createWithItem(menuToolsItem)
    menuTools:setPosition(pos[1], pos[2])
                
    layer:addChild(menuTools)    
    
    return true
end


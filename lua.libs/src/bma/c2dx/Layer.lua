-- bma/c2dx/Layer.lua

module(..., package.seeall)

Z_GAME = 0
Z_UI = 100
Z_DEBUG = 1000

TAG_DEBUG_LAYER = -10000
TAG_UI_LAYER    = -10001

function layer(scene, tag, z, createIfNotExists)    
    if scene == nil then 
        local SCENE = require("bma.c2dx.Scene")
        scene = SCENE.current(true) 
    end
    if createIfNotExists == nil then createIfNotExists = false end
    
    local l = scene:getChildByTag(tag)
    if l == nil and createIfNotExists then
        l = CCLayer:create()
        l.zOrder = z
        l:setTag(tag)
        scene:addChild(l)
    end
    return l
end

function debugLayer(scene, createIfNotExists)
    return layer(scene,TAG_DEBUG_LAYER,Z_DEBUG,createIfNotExists)
end

function uiLayer(scene, createIfNotExists)
    return layer(scene,TAG_UI_LAYER,Z_UI,createIfNotExists)
end
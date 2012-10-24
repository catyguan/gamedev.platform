-- bma/c2dx/Scene.lua

module(..., package.seeall)

currentScene = nil

function run(scene)
    currentScene = scene
    local sceneCur = CCDirector:sharedDirector():getRunningScene()
    if sceneCur ~= nil then
        CCDirector:sharedDirector():replaceScene(scene)    
    else
        CCDirector:sharedDirector():runWithScene(scene)
    end
end

function current(sure)
    if sure == nil then sure = false end
    local sceneCur = currentScene
    if sceneCur == nil then
        sceneCur = CCDirector:sharedDirector():getRunningScene()
    end
    if sceneCur == nil and sure then
        error("current scene is nil")
    end
    return sceneCur
end
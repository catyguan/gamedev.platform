-- bma/mgr/GMBase.lua
local Class = class.define("bma.mgr.GMBase",{})

function Class:runScene(clsName, ...)
	local obj = class.new(clsName)
	obj:init(...)
	local scene = obj:create()
	if scene==nil then
		if LOG:debugEnabled() then
			LOG:debug("GMBase",clsName.." create scene fail")
		end
		return
	end
	if LOG:debugEnabled() then
		LOG:debug("GMBase","runScene "..clsName)
	end
	director:runScene(scene)
end

function Class:unloadScene(clsName)
	
end
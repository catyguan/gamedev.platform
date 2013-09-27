-- bma/mgr/ActivityBase.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.mgr.ActivityBase",{})

local LDEBUG = LOG:debugEnabled()
local LTAG = "Activity"

function Class:install(gm)	
	if not self.id then error(self.className.." activity id is nil") end
		
	if gm.activities[self.id] then
		gm.activities[self.id]:uninstall(gn)
	end
	gm.activities[self.id] = self
	
	if LDEBUG then
		LOG:debug(LTAG, "install activity ["..self.id.."] ==> " .. self.className)
	end
	
	if self.initActivity then
		self:initActivity(gm)
	end
	
end

function Class:uninstall(gm)
	if not self.id then error("activity id is nil") end

	if gm.activities[self.id] == self then
		gm.activities[self.id] = nil
		
		if LDEBUG then
			LOG:debug(LTAG, "uninstall activity ["..self.id.."] ==> " .. self.className)
		end
		
		if self.closeActivity then
			self:closeActivity(gm)
		end
		
	end
end

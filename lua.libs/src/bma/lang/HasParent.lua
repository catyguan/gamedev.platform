-- bma/lang/HasParent.lua
require("bma.lang.ext.Core")

-- <<class part - HasParent>>
local Class = class.define("bma.lang.HasParent")

function Class:parent(v)
	if v~=nil then
		local cur = self:_parentGet()
		
		if cur and cur~=v then
			-- remove from old parent
			if cur.removeChild then
				cur:removeChild(self)
			else
				self:_parentSet(nil)
			end
		end
		
		if v then
			if v.addChild then
				v:addChild(self)			
			else
				self:iocParent(true)
				self:_parentSet(v)
			end
		end
	end
	return self:_parentGet()
end

function Class:_parentGet()
	return self._tempv.parent or self._prop.parent
end

-- used only by Parent
function Class:_parentSet(v)
	if v then
		if self:prop("iocParent") then
			self:prop("parent", v)
		else
			self:weak("parent", v)
		end
	else
		self:removeProp("parent")
		self:removeWeak("parent")
		self:removeProp("iocParent")
	end
	
end

function Class:iocParent(v)
	if v~=nil then		
		if v then
			self:prop("iocParent",v)
		else
			self:removeProp("iocParent")
		end
	end
	return self:prop("iocParent")	
end

function Class:afterRestoreObject()	
	if self:iocParent() then
		local p = self:_parentGet()
		if p and p.addChild then
			p:addChild(self)	
		end
	end
	class.superCall(self, Class, "afterRestoreObject")
end



-- bma/lang/HasChildren.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.Table")

-- <<class part - HasChildren>>
local Class = class.define("bma.lang.HasChildren")

function Class:ctor()
	self._hcProp = self.runv
	self.iocChildren = true
end

function Class:children()
	return self:_hcProp("children") or EMPTY_TABLE
end

function Class:addChild(o)
	if not o then return end
	local ch = self:_hcProp("children")
	if not ch then
		ch = {} 
		self:_hcProp("children", ch)
	end	
	if table.inArray(ch, o) then return	end
	table.insert(ch, o)
	if o.iocParent then
		if self.iocChildren then
			o:iocParent(true)
		else
			o:iocParent(false)
		end
	end
	if o._parentSet then
		o:_parentSet(self)	
	end		
end

function Class:isChild(o)
	local ch = self:_hcProp("children")
	if not ch then return false end
	return table.inArray(ch, o)
end

function Class:removeChild(o)
	if not o then return end
	local ch = self:_hcProp("children")
	if not ch then return end
	
	if table.removeArrayValue(ch, o, true) then
		if o._parentSet then
			o._parentSet(nil)	
		end	
	end
end

-- <<class part - HasChildrenStateful>>
local HasChildrenStateful = class.define("bma.lang.HasChildrenStateful" ,{Class})

function HasChildrenStateful:ctor()
	self._hcProp = self.prop
	self.iocChildren = nil
end

function HasChildrenStateful:afterRestoreObject()	
	if not self.iocChildren then
		local ch = self:_hcProp("children")
		if ch then
			for _,o in ipairs(ch) do
				if o._parentSet then
					o:_parentSet(self)
				end
			end
		end
	end
	class.superCall(self, Class, "afterRestoreObject")
end

-- bma/maxtrix/StatefulMatrix.lua
require("bma.matrix.Matrix")
require("bma.state.StatefulObject")

-- class: Matrix
local super = bma.matrix.Matrix
local Class = class.define("bma.matrix.StatefulMatrix", {super, bma.state.StatefulObject})

-- <<save/load>>
function Class:saveState(objState)
	-- save status
    objState.startTime = self.startTime
    objState.time = self.time

    -- save objects
    local olist = {}
    for k,v in pairs(self.objects) do
    	local data
    	if v.saveState then
    		data = {class = v.className, id = v:id()}    		    		
    	else
        	data = class.saveObject(v)
        end
        if data ~= nil then
            olist[k] = data
        end
    end
    objState.objects = olist
end

function Class:loadState(objState, cb)
	self:doload(objState, self.init, cb)
end

function Class:save()
    local r = {}
    self:saveState(r)
    return r
end

function Class:load(data, init)
	self:doload(data, init)
end

function Class:doload(data,init, callback)
    if init~=nil then
        init(self)
    end
    
    -- load status
    self.startTime = data.startTime
    self.time = data.time
    
    if LDEBUG then
        LOG:debug(LTAG,"load at [%s]", self:dumpTime())
    end
    
    -- load objects
    local SS
    local olist = data.objects
    local checkload = function()
    	if self.loading and #self.loading>0 then
    		return
    	end
    	if callback then
        	callback()        					
        end
    end
    if olist ~= nil then
        for k,sdata in pairs(olist) do
        	if sdata~=nil and sdata.class~=nil and sdata.id~=nil then
        		if not SS then SS = class.instance("bma.state.Service") end
        		if not self.loading then self.loading = {} end
        		local cb = function(err, obj)
        			if err then
        				LOG:warn(LTAG,"load objcet[%s] fail - %s",tostring(k), tostring(err))
        			else
        				self.objects[k] = obj
        				for i,v in ipairs(self.loading) do
        					if v==k then
        						table.remove(self.loading, i)
        						break
        					end
        				end
        				checkload()
        			end
        		end
        		table.insert(self.loading, k)
        		SS:get(cb, sdata.id, sdata.class)	
        	else
        		self:loadFromData(k, sdata)
        	end            
        end
    end
    checkload()
end

function Class:initState(objState)
	if self.init~=nil then
		self:init()
	end
	self:saveState(objState)
end 

function Class:isStateModify()
	return true
end

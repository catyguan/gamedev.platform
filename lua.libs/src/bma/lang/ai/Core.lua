-- bma/lang/ai/Core.lua
require("bma.lang.Class")

local LDEBUG = LOG:debugEnabled()
local Group = class.define("bma.lang.ai.Group")

function Group.logError(tag, msg)
	return function(err)
		if LDEBUG then 
			LOG:debug(tag, msg, tostring(err))
		end 
	end 
end

function Group:ctor(cb, errh, oneDone)
	self.callback = cb	
	self.errorHandler = errh
	self.oneDone = oneDone
	self.calls = {}
	self.finished = false	
end

function Group:call(k,f)
	local item = {k=k,f=f}
	table.insert(self.calls, item)
	return function(err, ...)
		-- print("callback", err, string.dump(arg))
		item.d = true
		if err then
			local rh
			if self.errorHandler then 
				rh = self.errorHandler(err)
			end
			if rh then 
				item.r = rh 
			else
				item.err = err
			end
		else			
			local r
			if item.f then 
				r = item.f(unpack(arg))
			else
				local c = #arg				
				if c==0 then				
				elseif c==1 then
					r = arg[1]
				else
					r = arg
				end			 
			end
			item.r = r
		end		
		self:checkAll()
	end
end

function Group:finishAll()
	self.finished = true
	self:checkAll() 
end

function Group:checkAll()
	if not self.finished then return end
		
	local f = function()
		local r = true
		for _,item in ipairs(self.calls) do
			if self.oneDone then
				if item.d then return true end
				r = false
			else
				if not item.d then return false end							
			end		 
		end
		return r
	end
	
	-- print("----checkAll: "..string.dump(self.calls).."-"..tostring(self.oneDone))
	if f() then
		-- print("checkAll done: "..string.dump(self:toTable()))
		aicall.safeDoneL(self.callback, nil, nil, self)		
		return true
	else
		-- print("checkAll fail: "..string.dump(self.calls))		
		return false
	end	
end

function Group:isAllSuccess()
	for _,item in ipairs(self.calls) do
		if not item.done then return false end
	end 
	return true
end

function Group:getKey(k)
	for _,item in ipairs(self.calls) do
		if item.k==k then return item.err,item.r end
	end	
end

function Group:getAt(i)
	local item = self.calls[i]
	if item then
		return item.err,item.r
	end
end

function Group:toTable(returnError)
	local r = {}
	for i,item in ipairs(self.calls) do
		local k = i
		if item.k then k = item.k end
		if item.err then
			if returnError then r[k] = item.err end	
		else
			r[k] = item.r
		end
	end
	return r
end

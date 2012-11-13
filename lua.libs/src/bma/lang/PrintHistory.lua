-- bma/lang/PrintHistory.lua

local org_print = print
local maxHistory = 100
local lastTime = 0
local seq = 1
local history = {}

print = function(...)
	while #history>maxHistory do
		table.remove(history)
	end
	local t = os.time()
	if t==lastTime then
		seq = seq + 1
	else
		lastTime = t
		seq = 1
	end
	table.insert(history, 1, {t=t,s=seq,p={...}})
    org_print(...)    
end

os.printHistory = function(t, seq)
	if t and seq then
		t = tonumber(t)
		seq = tonumber(seq)
		local r = {}
		for _,v in ipairs(history) do
			if v.t<t then break end
			if v.t==t and v.s<=seq then break end
			table.insert(r, v)
		end
		return r
	else
		return history
	end
end


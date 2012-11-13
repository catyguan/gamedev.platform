-- devui/Commands.lua

function devuiPing(t, seq)
	local r = appStatus()
	if os.printHistory then
		r['print'] = os.printHistory(t, seq)  
	end
	return r	
end
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")

include("bma.lang.Session")

local Session = bma.lang.Session

-- base
if true then
	local s = Session.new(10, 1000)
	-- var_dump(s)
	
	local obj = {}
	obj.onEntryRemove = function(key)
		print("removed",key)
	end
	
	for i=1,11 do
		s:set(i, obj)
	end
	print("start", s:dumpList())
	s:start()
end

require("bma.lang.ext.Dump")

if true then
	print("director", director)
	local o = director.userDefault
	print("userDefault", o)
	var_dump(o:getStringForKey("test1","<nil>"))
	var_dump(o:setStringForKey("test1",tostring(os.date())))
	var_dump(o:getStringForKey("test1","<nil>"))
end
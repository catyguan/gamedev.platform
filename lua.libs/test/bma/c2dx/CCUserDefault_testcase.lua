require("bma.lang.ext.Dump")

include("bma.c2dx.CCUserDefault")

if true then
	local cls = bma.c2dx.CCUserDefault;	
	
	print("UserDefault test")
	var_dump(cls.getForKey("test1","<nil>"))
	var_dump(cls.setForKey("test1",tostring(os.date())))
	var_dump(cls.getForKey("test1","<nil>"))
end
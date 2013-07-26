require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")

print(application, application.file)
local file = application.file
local TC = 1

if TC==1 then
	print("file.exists", file:exists("/global.dat"))
	print("file.load", file:load("/global.dat"))
	print("file.save", file:save("/global.dat", "hello"))
	print("file.exists", file:exists("/global.dat"))
	print("file.load", file:load("/global.dat"))
	print("file.delete", file:delete("/global.dat"))
end
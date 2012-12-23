-- bma/host/dotnet/DebugPrint.lua
print = function(...)
	dotnet.call("hostCall",1,"print",...)
end

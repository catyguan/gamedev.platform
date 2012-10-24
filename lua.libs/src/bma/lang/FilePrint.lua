-- bma/lang/FilePrint.lua

require("bma.lang.Class")

local Class = class.define("bma.lang.FilePrint")

function Class:ctor(filename, prefix)
	self.filename = filename
	self.prefix = prefix
end

function Class:print(...)
	local f
	if self.file == nil then
		local filename = string.format(self.filename, os.date(datePattern))
		f = io.open(filename, "a")
		if (f) then
			f:setvbuf ("no")
			self.file = f
		else
			error(string.format("file `%s' could not be opened for writing", filename))
		end
	else
		f = self.file
	end
	if self.prefix~=nil then
		f:write(self.prefix)
	end
	for i=1,arg.n do
		if i~=1 then f:write("\t") end
		local msg = arg[i]
		f:write(tostring(msg))
	end
	f:write("\n")
end

function Class:bind(both)
	self.org = print
	print = function(...)
		if both then
			self.org(...)
		end
		self:print(...)
	end
end
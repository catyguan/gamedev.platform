---- Core
require("bma.test.bootstrap")
require("bma.lang.ext.Dump")
include("bma.lang.Entity")

local EM = bma.lang.EntityManager

if true then
	-- serialize
	local Class1 = class.define("test.Class1")
	Class1.SERIALIZE = {
		hide=false
	}
	local Class2 = class.define("test.Class2",{Class1})
	local Class3 = class.define("test.Class3")

	local o1 = Class2.new()
	o1.hide = 123
	o1.a = true
	o1.b = {
		f1 = "asdad",
		f2 = {123456,234},
		f3 = Class3.new()
	}

	print("before:")
	var_dump(o1)

	print("saved:")
	local state
	local cb1 = function(err, data)
		if err then
			print("error", err)
		else
			var_dump(data)
			state = data
		end
	end 
	EM.storeObject(cb1, o1)
	
	if state then
		print("after:")
		local cb2 = function(err, o)
			if err then
				print("error", err)
			else
				var_dump(o)
			end
		end
		EM.restoreObject(cb2, state)
	end

end
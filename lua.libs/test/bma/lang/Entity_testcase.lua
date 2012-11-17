---- Core
require("bma.test.bootstrap")
require("bma.lang.ext.Dump")
include("bma.lang.Entity")

if true then
	-- serialize
	local Class1 = class.define("test.Class1", {bma.lang.Entity})	
	local Class2 = class.define("test.Class2",{Class1})
	local Class3 = class.define("test.Class3")

	function Class1:_restoreObject(cb)
		self.abc = true
		print("my restoreObject")		
		bma.lang.Entity._restoreObject(self, cb)		
	end
	function Class2:afterRestoreObject()
		print("afterRestoreObject")
	end

	local o1 = Class2.new()	
	o1:attr("a", true)
	o1:prop("b", {
		f1 = "asdad",
		f2 = {123456,234},
		f3 = Class3.new()
	});

	print("before:")
	var_dump(o1)

	print("saved:")	
	local state = class.saveObject(o1)
	var_dump(state)
	
	print("after:")
	local nobj = class.loadObject(state)
	var_dump(nobj)	
	
	local f1 = function(err, r)
		print("restore",err,r)
	end
	nobj:restoreObject(f1)
	
	print("afterRestore:")
	var_dump(nobj)
end
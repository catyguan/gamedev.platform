---- Core
package.loaded["bma.lang.ext.Core"] = nil
require("bma.lang.ext.Core")
package.loaded["bma.lang.Class"] = nil
require("bma.lang.Class")
require("bma.lang.ext.Dump")

if false then
	-- def class
	local Class1 = class.define("test.Class1")
	function Class1:test(w)
		print("hello",w)
	end

	var_dump(Class1)
	local o = Class1.new()
	var_dump(o)
	o:test("world")

	local Class2 = class.define("test.Class2",{Class1})

	var_dump(Class2)
	local o = Class2.new()
	var_dump(o)
	o:test("world")
end

if false then
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
		f2 = {123456},
		f3 = Class3.new()
	}

	print("before:")
	var_dump(o1)

	print("saved:")
	local data = class.saveObject(o1)
	var_dump(data)

	print("after:")
	local o2 = class.loadObject(data)
	var_dump(o2)
	test = o2

end

if false then
	local Class1 = class.define("test.Class1")
	local a = 0
	function Class1:ctor()
		a = a + 1
		self.a = a
	end
	var_dump(class.new("test.Class1"))
	var_dump(class.new("test.Class1"))
	var_dump(class.instance("test.Class1"))
	var_dump(class.instance("test.Class1"))
	
	class.setInstance("test.Class2","test.Class1")
	var_dump(class.instance("test.Class2"))
	
	class.setInstance("test.Class3",class.instance("test.Class1"))
	var_dump(class.instance("test.Class3"))
	
	var_dump(class.instance("bma.lang.Events"))
end

-- test class extends
if true then
	local C1 = class.define("test.c1")
	function C1:hello(w)		
		print("C1 hello ", w)
		class.superCall(self,C1,"hello", w)
	end 
	local C15 = class.define("test.c15")
	function C15:hello(w)		
		print("C1.5 hello ", w)
		class.superCall(self,C15,"hello", w)
	end
	local C2 = class.define("test.c2",{C15})
	function C2:hello(w)
		print("C2 hello ", w)
		class.superCall(self,C2,"hello", w)
	end
	
	local C3 = class.define("test.c3", {C2,C1})
	function C3:hello(w)
		print("C3 hello ", w)
		class.superCall(self,C3, "hello", w)
	end
	
	local o = C3.new()
	o:hello("world")
	
end

if false then
	-- test ref & refref
	local showtable = function(list)
		for k, v in pairs(list) do
			print(k , v)
		end
		print()
	end

	local list = {}
	setmetatable(list, {__mode = "v"})

	local f = function(list)
		local co1 = {id=1}
		local co2 = {id=2}
		local co3 = {id=3}
		co4 = {id=4}
		list[1] = co1
		list[2] = co2
		list[3] = co3
		list[4] = co4

		print("before free")
		showtable(list)

		co2.p1 = co1
		co1.p2 = co2

		collectgarbage()
		print("after free")
		showtable(list)
	end

	f(list)

	collectgarbage()
	print("outer")
	showtable(list)
end
require("bma.lang.ext.Dump")

if false then
	function test(...)
		print(#arg,arg[1])	
	end
	
	test(1,2,3,4)
end

if true then
	local a = {1,1,2,1}
	require("bma.lang.ext.Table")
	
	var_dump(a)
	
	local b = table.clone(a)
	table.removeArrayValue(b,1,true)
	var_dump(b)
	
	local b = table.clone(a)
	table.removeArrayValue(b,1,false)
	var_dump(b)
	
	
end
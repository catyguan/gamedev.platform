---- string
require("bma.lang.ext.Core")

testcase.include("bma.lang.ext.Table")
testcase.include("bma.lang.ext.Dump")

local t1 = {1,2,3,4,5,6}

if false then
	local t = {1,"abc",{a=1,b=2},true}
	var_dump(table.clone(t))
end

if true then
	local f2 = function(i,v)
		if v%2 ==0 then return true end
	end
	local f3 = function(i,v)
		if v%3 ==0 then return true end
	end

	local f = table.filter.AND(f2,f3)
	local tmp = table.clone(t1)
	table.walkRemove(tmp, f)
	var_dump(tmp)

	local f = table.filter.OR(f2,f3)
	local tmp = table.clone(t1)
	table.walkRemove(tmp, f)
	var_dump(tmp)
end

if true then

end
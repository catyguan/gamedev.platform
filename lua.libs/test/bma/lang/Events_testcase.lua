require("bma.lang.ext.Core")

testcase.include("bma.lang.Events")

local Events = bma.lang.Events

if true then
	local lis= Events.new()
	fun = function(msg)
		print(string.format("event -> %s",msg))
	end
	lis:listen(fun)
	lis:fire("hello world")
end

if true then
	local lh = Events.new()
	local lis1 = function(word)
		print("hello "..word)
	end
	local lis2 = function(word)
		print("hello2 "..word)
		return true, "bye"
	end
	print(lh:listen(lis1))
	print(lh:listen(lis2))
	print(lh:fire("world"))
	lh:clear()
	print(lh:size())
end
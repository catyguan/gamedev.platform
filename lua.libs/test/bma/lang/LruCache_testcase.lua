require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")

include("bma.lang.LruCache")

local LruCache = bma.lang.LruCache

-- base
if false then
	local c = LruCache.new(10, 1000)
	
	for i=1,11 do
		c:set(i, i)
		print(c:dumpList())
	end
	-- var_dump(c)
	
	print("show")
	for i=1,11 do
		print(i, c:get(i))
	end
	
	local st = os.time()
	while os.time()<=st+1 do end
	
	print("after")
	for i=1,11 do
		print(i, c:get(i))
	end
end

-- event - onEntryRemove
if true then
	local c = LruCache.new(10,1000)
	local obj = {}
	obj.onEntryRemove = function(key)
		print("remove",key)
	end
	
	print("no event")
	c:set("test", obj)
	c:touch("test")
	
	print("event")
	c:set("test", nil)
	
	print("event2")
	c:set("test2", obj)
	c:deleteLastEntry()
	
	print("event3")
	c:set("test3", obj)
	c:clear()
end

-- bma/lang/LruCache.lua
-- https://github.com/stephan-hof/mysql-proxy-in-memory-lru-cache/blob/master/in_memory_lru_cache.lua
require("bma.lang.ext.Core")

-- <<class - LruCache>>
local Class = class.define("bma.lang.LruCache")

-- The LRU-Cache is a table which maintains the insertion order of
-- values with a doubled linked list. Each new value is placed at the front of
-- this list. So the oldest values are found at the back/tail of this list.
-- A 'value' is the real data, which is going to be cached.
-- Each 'value' is wrapped by a 'entry' object.
-- This 'entry' keeps the pointers for the doubled linked list and also the lifetime.
function Class:ctor(maxsize, max_age)
	-- Number of current cache entries
	self.currentItems = 0
	-- Number of maximum cache entries
    self.maxsize = maxsize or 128
	-- Number of ms how long a cache entry should be valid
    self.maxAge = max_age or 0
	self.head = {key = "head", next_ = nil, prev_ = nil}
	self.tail = {key = "tail", next_ = nil, prev_ = nil}
	-- Stores the cache entries
	self.data = {}
    
    -- head/tail are nodes in the linked list, which are never be dropped.
    -- They are sentinels which make the operations on the linked list easier
    self.head.prev_ = self.tail
    self.tail.next_ = self.head
end

function Class:dumpList()
    local cur = self.tail
    local s = ""
    while cur ~= nil do
        s = s .. tostring(cur.key).."("..tostring(cur.maxAge)..")"
        cur = cur.next_
        if cur ~= nil then s = s .. ", " end
    end
    return s
end

function Class:addEntry(entry)
    self.head.prev_.next_ = entry
    entry.prev_ = self.head.prev_

    self.head.prev_ = entry
    entry.next_ = self.head

    self.currentItems = self.currentItems + 1
    self.data[entry.key] = entry
end

function Class:removeEntry(entry, s)
    self.currentItems = self.currentItems - 1
    self.data[entry.key] = nil

    entry.prev_.next_ = entry.next_
    entry.next_.prev_ = entry.prev_
    
    if not s and entry.value and type(entry.value)=="table" and entry.value.onEntryRemove then
    	pcall(function()
    		entry.value.onEntryRemove(entry.key)
    	end)
    end
end

function Class:size()
	return self.currentItems
end

function Class:deleteLastEntry()
    if self.currentItems ~= 0 then
        self:removeEntry(self.tail.next_)
    end
end

function Class:clear()
    while self.currentItems > 0 do
        self:removeEntry(self.tail.next_)
    end
end

function Class:set(key, value)
	local old = self.data[key]
	if value==nil then
		if old then
			self:removeEntry(old)
		end
		return
	end
	
    local entry = {
        -- If 'lru_cache_del_last_entry' is called the key is needed to remove
        -- the entry from cache.data
        key = key,
        value = value,
        maxAge = os.mclock() + self.maxAge,
        next_ = nil,
        prev_ = nil
    }
    if self.maxsize>0 and self.currentItems >= self.maxsize then
        self:deleteLastEntry()
    end
    if old then
    	self:removeEntry(old, old.value==value)
    end
    self:addEntry(entry)
end

function Class:touch(key)
	local old = self.data[key]
	if old then
		self:set(key, old.value)
	end
end

function Class:get(key)
    local entry = self.data[key]

    if entry == nil then
        return nil
    elseif self.maxAge>0 and entry.maxAge <= os.mclock() then
    	-- auto delete the entry because its too old to be kept
        self:removeEntry(entry)
        return nil        
    else
        -- Entry is there + young enough => Cache hit
        return entry.value
    end
end

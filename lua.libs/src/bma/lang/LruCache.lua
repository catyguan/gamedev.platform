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
    self.current_items = 0
	-- Number of maximum cache entries
    self.maxsize = maxsize or 128
	-- Number of ms how long a cache entry should be valid
    self.max_age = max_age or 0
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
        s = s .. tostring(cur.key)
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

    self.current_items = self.current_items + 1
    self.data[entry.key] = entry
end

function Class:removeEntry(entry)
    self.current_items = self.current_items - 1
    self.data[entry.key] = nil

    entry.prev_.next_ = entry.next_
    entry.next_.prev_ = entry.prev_
end

function Class:size()
	return self.current_items
end

function Class:deleteLastEntry()
    if self.current_items ~= 0 then
        self:removeEntry(self.tail.next_)
    end
end

function Class:set(key, value)
    local entry = {
        -- If 'lru_cache_del_last_entry' is called the key is needed to remove
        -- the entry from cache.data
        key = key,
        value = value,
        max_age = os.mclock() + self.max_age,
        next_ = nil,
        prev_ = nil
    }
    if self.current_items >= self.maxsize then
        self:deleteLastEntry()
    end
    local old = self.data[key]
    if old then
    	self:removeEntry(old)
    end
    self:addEntry(entry)
end

function Class:get(key)
    local entry = self.data[key]

    if entry == nil then
        return nil
    elseif self.max_age>0 and entry.max_age <= os.mclock() then
    	-- auto delete the entry because its too old to be kept
        self:removeEntry(entry)
        return nil        
    else
        -- Entry is there + young enough => Cache hit
        return entry.value
    end
end

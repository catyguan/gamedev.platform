-- bma/lang/Set.lua
module ("set", package.seeall)

function inSet(s, e)
	return rawget (s, e) == true
end

function insert(s, e)
	rawset(s, e, true)
end

function delete(s, e)
	rawset(s, e, nil)
end

--- Find the difference of two sets
-- @param s set
-- @param t set
-- @return s with elements of t removed
function diff(s, t)
	local r = new {}
	for e in pairs(s) do
		if not inSet(t, e) then
			insert (r, e)
		end
	end
	return r
end

--- Find the symmetric difference of two sets
-- @param s set
-- @param t set
-- @return elements of s and t that are in s or t but not both
function sdiff(s, t)
	return diff(union(s, t), interset(t, s))
end

--- Find the intersection of two sets
-- @param s set
-- @param t set
-- @return set intersection of s and t
function interset(s, t)
	local r = {}
	for e in pairs(s) do
		if inSet(t, e) then
			insert(r, e)
		end
	end
	return r
end

--- Find the union of two sets
-- @param s set
-- @param t set
-- @return set union of s and t
function union(s, t)
	local r = {}
	for e in pairs(s) do
		insert(r, e)
	end
	for e in pairs(t) do
		insert(r, e)
	end
	return r
end

--- Find whether one set is a subset of another
-- @param s set
-- @param t set
-- @return <code>true</code> if s is a subset of t, <code>false</code>
-- otherwise
function isSubset(s, t)
	for e in pairs(s) do
		if not inSet(t, e) then
			return false
		end
	end
	return true
end

--- Find whether one set is a proper subset of another
-- @param s set
-- @param t set
-- @return <code>true</code> if s is a proper subset of t, false otherwise
function isProperSubset (s, t)
	return isSubset(s, t) and not isSubset (t, s)
end

--- Find whether two sets are equal
-- @param s set
-- @param t set
-- @return <code>true</code> if sets are equal, <code>false</code>
-- otherwise
function isSame(s, t)
	return subset(s, t) and subset(t, s)
end

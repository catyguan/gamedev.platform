-- app/game1/Game.lua
require("bma.persistent.PersistentEntity")

local Class = class.define("app.game1.Game",{bma.persistent.PersistentEntity})

local G

function Class:ctor()
	G = self
end

function Class:initObject()	
	self:prop("number", -1)	
	self:prop("guest", -1)
	self:prop("done", false)
end

local check = function()
	local r	
	if G._p.guest > G._p.number then
		r = "it's too LARGE"
	elseif G._p.guest < G._p.number then
		r = "it's too SMALL"
	else
		G:prop("done", true)
		r = "it's SAME!!!!, use command 'restart' to restart a new game"
	end
	return r
end

function help()
	
	local r = ""
	r = r..""
	if G._p.number <0 then
		r = r.."A guest number game, current not start, please use command: 'setnumber XXX'"
	else
		if G._p.guest <0 then
			r = r.. "please use 'guest XXX' to guest"
		else
			r = r .. "current guest is ["..G._p.guest.."],"
			r = r .. check()
			if not G._p.done then	
				r = r .. ","
				r = r .. "please use 'guest XXX' to guest"
			end
		end
	end
	return r
end

function setnumber(v)
	if G._p.number<0 then
		if v<0 then return "invalid number" end		
		G:prop("number",v)
		return "ok, number is set, now use 'guest XXX' start to guest"
	else
		return "number is set, please use 'guest XXX'"
	end
end


function guest(v)
	if G._p.done then
		return "already guest the number["..G._p.number.."]"
	end
	
	if G._p.number>0 then
		if v<0 then return "invalid number" end		
		G:prop("guest",v)
		return check()
	else
		return "number not set, please use 'setnumber XXX'"
	end
end

function restart()
	if not G._p.done then
		return "can't restart"
	end
	G:initObject()
	return "ok"
end
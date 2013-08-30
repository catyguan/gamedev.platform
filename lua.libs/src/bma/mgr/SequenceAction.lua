-- bma/mgr/SequenceAction.lua
local Class = class.define("bma.mgr.SequenceAction", {})

local LDEBUG = LOG:debugEnabled()
local LTAG = "SequenceAction"

function Class:ctor()
	
end

function Class:processActions(sess)
	if V(sess.stop, false) then
		return
	end
	if #sess.actions >0 then		
		act = table.remove(sess.actions, 1)
		local name = act[1]
		sess.current = name
		local fn = self["run_"..name]
		if fn~=nil then
			local f = sess.nextf
			if #sess.actions>0 then
				local this = self
				f = function()
					self:processActions(sess)
				end
			else
				f = sess.nextf
			end
			return fn(self, f, unpack(act, 2))			
		else
			LOG:warn(LTAG, "unknow action '"..act.."'")
			sess.nextf()
		end
	else
		sess.nextf()
	end		
end

function Class:runActions(nextf, actions)
	if type(actions[1])=="string" then
		actions = {actions}
	end
	local sess = {
		actions=actions,
		nextf=nextf,
	}
	self:processActions(sess)
	return sess
end

function Class:stopActions(sess)
	sess.stop = true
	if sess.current then
		local fn = self["stop_"..sess.current]
		if fn~=nil then
			return fn(self)		
		end
	end	
end
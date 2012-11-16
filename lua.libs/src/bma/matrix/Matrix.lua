-- bma/maxtrix/Matrix.lua
require("bma.persistent.PersistentEntity")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Table")

-- macro
local _statis = {}

MT = function(m)
    if not m then
        return _statis.cur
    end
    return m
end

NOW = function()
    return _statis.cur:prop("time")
end

DELAY = function(t)
    return _statis.cur:prop("time") + t
end

OBJ = function(oid)
    return _statis.cur:obj(oid)
end

local CFG = CONFIG.Matrix or EMPTY_TABLE
local LDEBUG = LOG:debugEnabled()
local LACTDEBUG = CFG.ActionDebug or false
local LTAG = "Matrix"

-- class: Matrix
local Matrix = class.define("bma.matrix.Matrix", {bma.persistent.PersistentEntity})

Matrix.TLF_SECOND = function(t,st)
    return string.format("%.3fs",(t-st)/1000)
end

function Matrix:ctor()    
    self.timeLabelFun = function(t,st)
        return (t-st) .. "t"
    end
    self._p = {}
    self.actions = {}
    self.hasNewAction = false    
    self.config = {}
    setmetatable(self.config,{__index=
			function(t,k)
				return CFG[k]
			end
		})
end

function Matrix:initObject()
	self:prop("startTime", 0)
    self:prop("time", 0)
    self:prop("begin", false)
    self:prop("objects", {})
end

function Matrix:isObjectValid()
	return self._p.objects ~= nil
end

function Matrix:isBegin()
	return self._p.begin
end

function Matrix:dumpTime()
    return self.timeLabelFun(self._p.time, self._p.startTime)
end

function Matrix:clock()
    return self._p.time + self._p.startTime
end

function Matrix:random(m,n)
    if self.randomFun ~= nil then return self.randomFun(m,n) end
    return math.random(m,n)
end

function Matrix:randomseed(x)
    if self.randomseedFun ~= nil then return self.randomseedFun(x) end
    return math.randomseed(x)
end

-- <<objects>>
function Matrix:obj(oid)    
    if oid==nil then return nil end
    return self._p.objects[oid]
end

function Matrix:addObject(o)    
    assert(o)
    local oid = o:id()
    if oid==nil then
        error(string.format("object(%s) id is nil",tostring(o)))
    end
    if self._p.objects[oid]~=nil then
        error(string.format("oid[%s] exists",tostring(oid)))
    end
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> object[%s] enter",self:dumpTime(), self._p.time, oid)
    end
    self._p.objects[oid] = o
    self:stateModify(true)
    if V(self:isBegin(), false) and o.onMatrixStart then    
        o:onMatrixStart(false)
    end
    return o
end

function Matrix:loadObject(cb, id, param, syn)
	local m = class.instance("bma.persistent.Service")
	local cb1 = function(err, o)
		if not err then
			if o and o:isObjectValid() then
				self:addObject(o)
				return aicall.done(cb, nil, self)
			end
			cb("loadObject("..tostring(id)..") invalid object")
		else
			cb(err)
		end
	end
	m:get(cb1, id, param, syn)
end

function Matrix:removeObject(oid)
    local o = self._p.objects[oid]
    if o~=nil then
        if LDEBUG then
            LOG:debug(LTAG,"%s(%d) >> object(%s) leave",self:dumpTime(), self._p.time, oid)
        end
        if o.onMatrixKill ~= nil then
            o:onMatrixKill(true)
        end
        self._p.objects[oid] = nil 
        self:stateModify(true)   
        return true
    end
    return false
end

-- <<time & actions>>
local actstr = function(a)
    local s = ""
    if type(a)=="table" and a.className~=nil then
        s = a.className.." - "
    end
    s = s .. string.dump(a)
    return s
end
function Matrix:dumpAction()
    local r = ""
    for _,v in ipairs(self.actions) do 
        if #r > 0 then r = r .. "\n" end
        r = r .. string.format("%s - %s",self.timeLabelFun(v.t, self._p.startTime),actstr(v.a))
    end
    return r
end

local handleAction = function(self, a)
    
    self:prop("time", a.t)        
    if a.a == nil then
        return
    end
            
    if LDEBUG and LACTDEBUG then
        LOG:debug(LTAG,"%s(%d) >> take[%s]",self:dumpTime(), self._p.time,actstr(a))
    end                                
    local done,r,stt = pcall(function()
        if type(a.a)=="function" then
            a.a()
        elseif a.a.executeAction then
            a.a:executeAction()
        else
        	LOG:warn(LTAG,"unable to execute action '%s'", tostring(a.a))
        end
    end)
    if not done then        
        LOG:warn(LTAG,"execute action fail\n%s\n%s", r,stt)
    end
    
end

function Matrix:process(step)
    if step==nil then step = 0 end
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> process(%d)",self:dumpTime(), self._p.time, step)
    end
    _statis.cur = self
    
    local et = self._p.time + step
    while #self.actions>0 do
        if self.hasNewAction then            
            table.sort(self.actions, function(e1,e0)
                return e1.t < e0.t
            end)
            self.hasNewAction = false
        end
        local a = self.actions[1]
        if a.t <= et then
            table.remove(self.actions,1)
            handleAction(self, a)            
        else
            break     
        end
    end    
    self:prop("time", et)
    return self._p.time

end

function Matrix:delayAction(time,act)    
    self:runAction(self._p.time+time,act)
end

function Matrix:nowAction(act)    
    self:runAction(self._p.time,act)
end

function Matrix:runAction(time, act)    
    if act==nil then
        return false
    end    
    if time < self._p.time then
        if LOG:warnEnabled() then
            LOG:warn(LTAG,"%d >> runAction(%d,_,%s) invalid time",self._p.time,time,actstr(act))
        end
        error(string.format("runAction(%d) invalid time at %d", time, self._p.time))
    end
    table.insert(self.actions, {t=time,a=act})
    self.hasNewAction = true    
    return true
end

function Matrix:getAction(f)
    return table.select(self.actions, function(k,v)
            return f(k,v.a)
        end)
end

function Matrix:getActionOne(f)
    return table.selectOne(self.actions, function(k,v)
            return f(k,v.a)
        end)
end

function Matrix:removeAction(f)
    for i,v in ipairs(self.actions) do
        if f(k,v.a) then
            table.remove(self.actions,i)            
        end
    end
end

-- <<setup & begin>>
function Matrix:setup(startTime, time)
    self:prop("startTime", startTime)
    self:prop("time", time)
    _statis.cur = self    
end

function Matrix:begin()
    self:prop("begin",true)
    _statis.cur = self
    
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> begin",self:dumpTime(), self._p.time)    
    end
    
    for k,o in pairs(self._p.objects) do
        if o.onMatrixStart~= nil then
            o:onMatrixStart(false)
        end
    end
end
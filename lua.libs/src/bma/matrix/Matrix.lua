-- bma/maxtrix/Matrix.lua

require("bma.persistent.PersistentEntity")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Table")
require("bma.lang.Events")

-- macro
local _statis = {}

MT = function(m)
    if m==nil then
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

local CFG = CONFIG.Matrix or {}
local LDEBUG = LOG:debugEnabled()
local LCMDDEBUG = CFG.CommandDebug or LDEBUG
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
    self.commands = {}
    self.hasNewCommand = false    
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

-- <<time & commands>>
local cmdstr = function(c)
    local s = ""
    if type(c)=="table" and c.className~=nil then
        s = c.className.." - "
    end
    s = s .. string.dump(c)
    return s
end
function Matrix:dumpCommand()
    local r = ""
    for _,v in ipairs(self.commands) do 
        if #r > 0 then r = r .. "\n" end
        r = r .. string.format("%s - %s",self.timeLabelFun(v.t, self._p.startTime),cmdstr(v.c))
    end
    return r
end

local handleCommand = function(self, a)
    
    self:prop("time", a.t)        
    if a.c == nil then
        return
    end
            
    if LDEBUG and LCMDDEBUG then
        LOG:debug(LTAG,"%s(%d) >> take[%s]",self:dumpTime(), self._p.time,cmdstr(a))
    end                                
    local done,r,stt = pcall(function()
        if type(a.c)=="function" then
            a.c()
        else
            a.c:executeCommand()
        end
    end)
    if not done then        
        LOG:warn(LTAG,"process command fail\n%s\n%s", r,stt)
    end
    
end

function Matrix:process(step)
    if step==nil then step = 0 end
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> process(%d)",self:dumpTime(), self._p.time, step)
    end
    _statis.cur = self
    
    local et = self._p.time + step
    while #self.commands>0 do
        if self.hasNewCommand then            
            table.sort(self.commands, function(e1,e0)
                return e1.t < e0.t
            end)
            self.hasNewCommand = false
        end
        local a = self.commands[1]
        if a.t <= et then
            table.remove(self.commands,1)
            handleCommand(self, a)            
        else
            break            
        end
    end    
    self:prop("time", et)
    return self._p.time

end

function Matrix:delayCommand(time,cmd)    
    self:runCommand(self._p.time+time,cmd)
end

function Matrix:nowCommand(cmd)    
    self:runCommand(self._p.time,cmd)
end

function Matrix:runCommand(time,cmd)    
    if cmd==nil then
        return false
    end    
    if time < self._p.time then
        if LOG:warnEnabled() then
            LOG:warn(LTAG,"%d >> runCommand(%d,_,%s) invalid time",self._p.time,time,cmdstr(cmd))
        end
        error(string.format("runCommand(%d) invalid time at %d", time, self._p.time))
    end
    table.insert(self.commands, {t=time,c=cmd})
    self.hasNewCommand = true    
    return true
end

function Matrix:getCommand(f)
    return table.select(self.commands, function(k,v)
            return f(k,v.c)
        end)
end

function Matrix:getCommandOne(f)
    return table.selectOne(self.commands, function(k,v)
            return f(k,v.c)
        end)
end

function Matrix:removeCommand(f)
    for i,v in ipairs(self.commands) do
        if f(k,v.c) then
            table.remove(self.commands,i)            
        end
    end
end

-- <<setup & begin & save/load>>
function Matrix:setup(startTime, time, init, creator)
    self:prop("startTime", startTime)
    self:prop("time", time)
    _statis.cur = self
    
    if init ~= nil then
        init(self)
    end
    if creator ~= nil then
        creator(self)
    end
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

-- <<class - Command>>
local MatrixCommand = class.define("bma.matrix.MatrixCommand", {bma.lang.StdObject})

function MatrixCommand:id(v)
    return self:attr("id",v)
end

function MatrixCommand:value(p1,p2)    
    return self:runv(p1,p2)
end

function MatrixCommand:isCancel()
    return V(self:value("hasCancel"),false)
end

function MatrixCommand:cancel()
    self:value("hasCancel",true)
end

function MatrixCommand:execute()
    return false, 0
end

function MatrixCommand:handleExecute()
    self:execute()    
end

function MatrixCommand:executeCommand()
    if self:isCancel() then return end
    self:handleExecute()
end

function MatrixCommand:runNow()
    MT():nowCommand(self)
end

function MatrixCommand:runAt(tm)
    MT():runCommand(tm, self)
end

function MatrixCommand:runDelay(tm)
    MT():delayCommand(tm, self)
end
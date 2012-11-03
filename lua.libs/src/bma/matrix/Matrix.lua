-- bma/maxtrix/Matrix.lua

require("bma.lang.Class")
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
    return _statis.cur.time
end

DELAY = function(t)
    return _statis.cur.time + t
end

OBJ = function(oid)
    return _statis.cur:obj(oid)
end

table.filter.BY_ID = function(id)
    return function(k,v)
        if v==nil then return false end
        if v.id==nil then return false end
        return v:id()==id
    end
end

local LDEBUG = LOG:debugEnabled()
local LCMDDEBUG = true
local LTAG = "Matrix"

-- << class - MatrixObject >>
local MatrixObject = class.define("bma.matrix.MatrixObject",{})

MatrixObject.EVENT = {}

MatrixObject.defEvent = function(k,v)
    if MatrixObject.EVENT[k] ~= nil then
        error("event["..k.."] exists")
    end
    MatrixObject.EVENT[k] = v
end

-- abtract
function MatrixObject:id(v)
    if v~=nil then self._id = v end
    return self._id
end

-- abtract
function MatrixObject:parent(v)
    if v~=nil then 
        if type(v)=="table" then
            self._parent = v
        else
            self._parent = nil
        end
    end
    return self._parent
end

-- abtract
function MatrixObject:events(s)
    if self._events==nil and s then 
        self._events = bma.lang.Events.new()
    end
    return self._events
end

-- abstract
function MatrixObject:onMatrixStart(reload)
end

-- abtract
function MatrixObject:onMatrixKill(del)
    local e = self:events()
    if e~=nil then
        e:clear()
    end
end

function MatrixObject:childId()
    if self._childId==nil then 
        self._childId = 1
    else
        self._childId = self._childId + 1        
    end
    return self._childId
end

function MatrixObject:listen(lis, id) 
    local e = self:events(true)
    e:listen(lis, id)
end

function MatrixObject:fireEvent(ev)    
    if ev==nil then return end
    if ev.source==nil then ev.source = self end
    local e = self:events()
    if e~=nil then
        e:fire(ev)
    end
    local p = self:parent()
    if p~=nil then
        p:fireEvent(ev)
    else
        local mt = MT()
        if mt~=nil and mt~=self then
            mt:fireEvent(ev)
        end
    end
end

-- class: Matrix
local Matrix = class.define("bma.matrix.Matrix", {bma.matrix.MatrixObject})

Matrix.TLF_SECOND = function(t,st)
    return string.format("%.3fs",(t-st)/1000)
end

function Matrix:ctor()    
    self.timeLabelFun = function(t,st)
        return (t-st) .. "t"
    end
    self.hasBegin = false
    self.startTime = 0
    self.time = 0
    self.commands = {}
    self.hasNewCommand = false
    self.objects = {}
    self.config = {}
    setmetatable(self.config,{__index=
			function(t,k)
			    if CONFIG.Matrix == nil then return nil end
				return CONFIG.Matrix[k]
			end
		})
end

function Matrix:dumpTime()
    return self.timeLabelFun(self.time,self.startTime)
end

function Matrix:clock()
    return self.time + self.startTime
end

function Matrix:random(m,n)
    if self.randomFun ~= nil then return self.randomFun(m,n) end
    return math.random(m,n)
end

function Matrix:randomseed(x)
    if self.randomseedFun ~= nil then return self.randomseedFun(x) end
    return math.randomseed(x)
end

NEWCID = function(o)
    if o~=nil and o.id~=nil and o.childId~=nil then 
        return o:id().."-"..o:childId()    
    end
    return MT():childId()
end

-- <<objects>>
function Matrix:obj(oid)    
    if oid==nil then return nil end
    return self.objects[oid]
end

function Matrix:addObject(o)    
    assert(o)
    local oid = o:id()
    if oid==nil then
        error(string.format("object(%s) id is nil",tostring(o)))
    end
    if self.objects[oid]~=nil then
        error(string.format("oid[%s] exists",tostring(oid)))
    end
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> object[%s] enter",self:dumpTime(), self.time, oid)
    end
    self.objects[oid] = o
    if V(self.hasBegin,false) and o.onMatrixStart then    
        o:onMatrixStart(false)
    end
    return o
end

function Matrix:removeObject(oid)
    local o = self.objects[oid]
    if o~=nil then
        if LDEBUG then
            LOG:debug(LTAG,"%s(%d) >> object(%s) leave",self:dumpTime(), self.time, oid)
        end
        if o.onMatrixKill ~= nil then
            o:onMatrixKill(true)
        end
        self.objects[oid] = nil    
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
        r = r .. string.format("%s - %s",self.timeLabelFun(v.t, self.startTime),cmdstr(v.c))
    end
    return r
end

local handleCommand = function(self, a)
    
    self.time = a.t        
    if a.c == nil then
        return
    end
            
    if LDEBUG and LCMDDEBUG then
        LOG:debug(LTAG,"%s(%d) >> take[%s]",self:dumpTime(), self.time,cmdstr(a))
    end                                
    local done,r,stt = pcall(function()
        if type(a.c)=="function" then
            a.c()
        else
            a.c:doExecute()
        end
    end)
    if not done then        
        LOG:warn(LTAG,"process command fail\n%s\n%s", r,stt)
    end
    
end

function Matrix:process(step)
    if step==nil then step = 0 end
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> process(%d)",self:dumpTime(), self.time, step)
    end
    _statis.cur = self
    
    local et = self.time + step
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
    self.time = et
    return self.time

end

function Matrix:delayCommand(time,cmd)    
    self:runCommand(self.time+time,cmd)
end

function Matrix:nowCommand(cmd)    
    self:runCommand(self.time,cmd)
end

function Matrix:runCommand(time,cmd)    
    if cmd==nil then
        return false
    end    
    if time < self.time then
        if LOG:warnEnabled() then
            LOG:warn(LTAG,"%d >> runCommand(%d,_,%s) invalid time",self.time,time,cmdstr(cmd))
        end
        error(string.format("runCommand(%d) invalid time at %d", time, self.time))
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
    self.startTime = startTime
    self.time = time
    _statis.cur = self
    
    if init ~= nil then
        init(self)
    end
    if creator ~= nil then
        creator(self)
    end
end

function Matrix:begin()
    self.hasBegin = true
    _statis.cur = self
    
    if LDEBUG then
        LOG:debug(LTAG,"%s(%d) >> begin",self:dumpTime(), self.time)    
    end
    
    for k,o in pairs(self.objects) do
        if o.onMatrixStart~= nil then
            o:onMatrixStart(false)
        end
    end
end

function Matrix:save()
    local r = {}
    -- save status    
    r.startTime = self.startTime
    r.time = self.time

    -- save objects
    local olist = {}
    for k,v in pairs(self.objects) do        
        local data = class.saveObject(v)
        if data ~= nil then
            olist[k] = data
        end
    end
    r.objects = olist
end

function Matrix:load(data,init)
    if init~=nil then
        init(self)
    end
    
    -- load status
    self.startTime = data.startTime
    self.time = data.time
    
    if LDEBUG then
        LOG:debug(LTAG,"load at [%s]", self:dumpTime())
    end
    
    -- load objects
    local olist = data.objects
    if olist ~= nil then
        for k,sdata in pairs(olist) do
            local o = nil
            if sdata ~= nil then
                local done,r,stt = pcall(function(th,data)
                    return class.loadObject(data)
                end,self,sdata)                
                if not done then
                    LOG:warn(LTAG,"newInstance[%s -- %s] fail\n%s\n%s", k, tostring(sdata.class), r, stt)                    
                else 
                    o = r
                end
            end
            if o ~= nil then
                self.objects[k] = o
            else
                LOG:warn(LTAG,"load objcet[%s] fail",tostring(k))
            end
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

function MatrixCommand:doExecute()
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
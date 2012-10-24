-- ge/daemon/Command.lua

require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Table")

require("ge.daemon.Matrix")

local Matrix = ge.daemon.Matrix
local MatrixCommand = ge.daemon.MatrixCommand

local LDEBUG = LOG:debugEnabled()
local LTAG = "Command"

-- <<class part - Target>>
local CommandSource = class.define("ge.daemon.command.CommandSource")
function CommandSource:linkCommand(cmd)
    local tag = cmd
    if type(cmd)=="table" then
        tag = cmd:id()
        if tag==nil then
            tag = cmd:id(NEWCID(self))
        end
    end
    local ctag = self:runv("commandTag")
    if ctag==nil then
        self:runv("commandTag",tag)
    elseif type(ctag)=="table" then
        table.insert(ctag, tag)
    else
        self:runv("commandTag", {ctag, tag})
    end
end

function CommandSource:unlinkCommand()
    local ctag = self:runv("commandTag")
    if ctag==nil then return end
    local m = MT()
    if type(ctag)=="table" then
        for _,tag in ctag do
            m:removeCommand(table.filter.BY_ID(tag))
        end        
    else
        m:removeCommand(table.filter.BY_ID(ctag))
    end
    self:removeRunv("commandTag")
end

-- <<class part - Target>>
local Target = class.define("ge.daemon.command.Target")
function Target:createTarget(o)    
    self:value("target", o)
    return self
end

function Target:target()
    return self:value("target")
end

function Target:id(v)
    local r = self:attr("id",v)
    if v==nil and r==nil then
        local o = self:target()
        if o~=nil then
            r = self:attr("id",NEWCID(o))
        end
    end
    return r
end

-- <<class part - ObjectCall>>
local ObjectCall = class.define("ge.daemon.command.ObjectCall", {Target})
function ObjectCall:createCall(o,m,...)    
    self:createTarget(o)
    self:value("method",m)
    self:value("arg",arg)
    return self
end

function ObjectCall:invoke(...)
    local o = self:target()
    if o==nil then        
        return false
    end    
    local m = self:value("method")
    if o[m] == nil then
        LOG:warn(LTAG,"miss method [%s]", oid)
        return false
    end    
    local ps = self:value("arg")
    local cps = {}
    if ps ~= nil then
        cps = table.clone(ps)
    end
    if arg ~= nil then        
        for i,v in ipairs(arg) do
            table.insert(cps,i,v)
        end
    end
    return o[m](o,unpack(cps))    
end

function ObjectCall:execute()
    return self:invoke()
end

table.filter.CMD_CALL = function(oid,m)
    return function(k,v)
        if v~=nil and v:value("oid")==oid and v:value("method")==m then return true end
        return false;
    end
end

-- <<class - CallCommand>>
local CallCommand = class.define("ge.daemon.command.CallCommand",{ObjectCall, MatrixCommand})

function ge.daemon.MatrixObject:callCommand(m,...)
    local r = CallCommand.new()
    r:createCall(self,m,...)
    return r
end

-- <<class - GroupCommand>>
local GroupCommand = class.define("ge.daemon.command.GroupCommand",{MatrixCommand})

function GroupCommand:children(s)
    local chl = self:attr("children")
    if chl==nil and V(s,false) then
        chl = self:attr("children",{})
    end
    return chl
end

function GroupCommand:addCommand(tag, cmd)
    if cmd==nil then return end    
    local chl = self:children(true)
    table.insert(chl,cmd)
    if tag~=nil then cmd:id(tag) end
end

function GroupCommand:getCommand(tag)
    local chl = self:children()
    if chl == nil then return nil end
    for _,v in ipairs(chl) do
        if v:id()==tag then return v end
    end
    return nil
end

function GroupCommand:removeCommand(tag)
    local chl = self:children()
    if chl == nil then return nil end
    for i,v in ipairs(chl) do
        if v:id()==tag then 
            table.remove(chl,i)
            return
        end
    end
    return nil
end

function GroupCommand:getFirstCommand()
    local chl = self:children()
    if chl==nil then return nil end
    return chl[1]
end

-- <<class - WrapCommand>>
local WrapCommand = class.define("ge.daemon.command.WrapCommand",{GroupCommand})

function WrapCommand:wrap(cmd)
    self:addCommand("wrap",cmd)
    return self
end

function WrapCommand:isCancel()
    local cmd = self:getFirstCommand()
    if cmd ~= nil and cmd:isCancel() then
        return true
    end
    return MatrixCommand.isCancel(self)
end

function WrapCommand:execute()
    local cmd = self:getFirstCommand()
    if cmd ~= nil then
        return cmd:execute()
    end
    return false
end

-- << class - SequenceCommand >>
local SequenceCommand = class.define("ge.daemon.command.SequenceCommand",{GroupCommand})
function SequenceCommand:execute()
    local chl = self:children()
    if chl == nil then return false end
    local s = V(self:value("active"),1) 
    for idx=s, #chl do
        self:value("active", idx)
        local cmd = chl[idx]
        local r = cmd:handleExecute()
        if V(r,false) then
            return r
        end
    end
    return false
end

-- << class - Repeat >>
local RepeatCommand = class.define("ge.daemon.command.RepeatCommand",{MatrixCommand})

function RepeatCommand:createRepeat(interval,times)
    self:value("interval",V(interval,1000))
    self:value("times",times)
end

function RepeatCommand:handleExecute()    
    local r = MatrixCommand.handleExecute(self)
    if not V(r,true) then
        return false
    end    
    local ival = self:value("interval")
    local times = self:value("times")
    if V(times,0)>0 then
        local tick = V(self:runv("tick"),1)
        if tick>=times then
            return
        end
        self:runv("tick",tick+1)
    end
    MT():delayCommand(ival, self)
end

-- << class - Duration >>
local Duration = class.define("ge.daemon.command.Duration")

function Duration.getNextTime(now,endt,ival)
    local last = false
    if now == endt then last = true end    
    local rival = ival
    if now+ival>endt then
        rival = endt - now
    end
    return rival, last
end

function Duration:createDuration(start,endt,interval)
    self:value("startTime",start)
    self:value("endTime",endt)
    self:value("interval",V(interval,1000))
end

function Duration:run()
    MT():runCommand(self:value("startTime"), self)
end

function Duration:handleExecute()
    local endt = self:value("endTime")
    local ival = self:value("interval")
    
    local tick = V(self:value("tick"),0)+1
    self:value("tick", tick)
    
    local now = MT().time
    local rival,last = Duration.getNextTime(now, endt, ival)
    
    local r = self:execute(tick, last)
    if not V(r,true) then
        return false
    end
    
    if last then        
        return false
    end         
    
    MT():delayCommand(rival, self)
    return true    
end

-- << class - DurationCommand >>
local DurationCommand = class.define("ge.daemon.command.DurationCommand",{Duration,MatrixCommand})

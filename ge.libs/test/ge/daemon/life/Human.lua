-- 模拟人生测试用例
require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")
require("bma.lang.ext.Table")

require("ge.daemon.Matrix")
require("ge.daemon.Command")
require("ge.daemon.State")
require("ge.daemon.Thing")

local Matrix = ge.daemon.Matrix
local Thing = ge.daemon.Thing
local MoveCommand = ge.daemon.command.MoveCommand
local Duration = ge.daemon.command.Duration

local LTAG = "Human"

-- << State - Idle >>
local IdleState = class.define("test.state.IdleState",{ge.daemon.state.ObjectState})
function IdleState:ctor()
    self.name = "idle"
end

function IdleState:activeCommand()
    local cmd = self:performCommand()
    cmd.tag = NEWTAG()
    MT():delayCommand(1,cmd)
    return cmd.tag
end

function IdleState:process(ret)
    local me = self:me()
    local now = NOW()
    
    LOG:debug(LTAG,"idle...")
    
    return true
end

-- << State - Sleep >>
local SleepState = class.define("test.state.SleepState",{ge.daemon.state.ObjectState})
function SleepState:ctor()
    self.name = "sleep"
    self.busy = {"study","work"}
end

function SleepState:createSleep(sleepTime)
    self.wakeTime = NOW()+sleepTime    
end

function SleepState:activeCommand()
    local cmd = self:performCommand()
    cmd.tag = NEWTAG()
    MT():delayCommand(3600/2,cmd)
    return cmd.tag
end

function SleepState:process(ret)
    local me = self:me()
    local now = NOW()
    local endt = self.wakeTime
    local ival = 3600/2
    
    local nt,last = Duration.getNextTime(now, endt, ival)
    if last then
        LOG:debug(LTAG,"wake up")
        return false          
    end
    
    -- restore energy
    me:addProp("energy",5,0,"maxEnergy","sleep")
    ret.resp = true
    ret.nextTime = nt
    return true
end

-- << State - Work >>
local WorkState = class.define("test.state.WorkState",{ge.daemon.state.ObjectState})
function WorkState:ctor()
    self.name = "sleep"
    self.busy = {"study","work"}
end

function WorkState:createSleep(sleepTime)
    self.wakeTime = NOW()+sleepTime    
end

function WorkState:activeCommand()
    local cmd = self:performCommand()
    cmd.tag = NEWTAG()
    MT():delayCommand(3600/2,cmd)
    return cmd.tag
end

function WorkState:process(ret)
    local me = self:me()
    local now = NOW()
    local endt = self.wakeTime
    local ival = 3600/2
    
    local nt,last = Duration.getNextTime(now, endt, ival)
    if last then
        LOG:debug(LTAG,"wake up")
        return false          
    end
    
    -- restore energy
    me:addProp("energy",5,0,"maxEnergy","sleep")
    ret.resp = true
    ret.nextTime = nt
    return true
end

-- << Class - Human >>
local Human = class.define("test.Human", {Thing})
function Human:ctor()
end

function Human:create()
    -- 存款
    self:prop("money", 100)
    -- 体力
    self:prop("energy",80)
    self:prop("maxEnergy",100)
    -- 能力
    self:prop("work", 10)
    -- 智力
    self:prop("iq", 10)
    -- 职业
    self:prop("job","")
    -- 职业经验
    self:prop("jobExp",0)
    
    local main = ge.daemon.state.State.new()
    main.name = "main"    
    self:addState(main)
end

function Human:sleep(sleepTime)
    if self:hasState("sleep") then
        return
    end
    
    local st = SleepState.new()
    st:bind(self,nil,"main")
    st:createSleep(sleepTime)
        
    self:getState("main"):pushState(st)
end

-- 赛车模拟测试用例
require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")
require("bma.lang.ext.Table")

testcase.include("ge.daemon.Matrix")
testcase.include("ge.daemon.Thing")
testcase.include("ge.daemon.Living")
testcase.include("ge.daemon.HFSM")

local Matrix = ge.daemon.Matrix
local Living = ge.daemon.Living

-- << Class - Player >>
local Player = class.define("racing.Player", {Living})
function Player:ctor()
    -- self:prop("",111)
end
function Player:drive(car)
    self:prop("car", car:ref())
    car:prop("driver", self.oid)
end
function Player:enterMatrix()
    print("here")
    self:startHeartBeat()
end
function Player:thinkAndDo()
    local car = self:prop('car')
    if car:isBusy("speedUp") then
        return
    end
    
    local sup = 0
    if V(car:prop("speed"),0)==0 then
        sup = 1        
    else        
        local match = MT():obj("match")
        local carpos = car:prop("pos")
        local sp = car:prop("speed")
        local cp1 = match:checkpoint(carpos)                
        local dan1 = 100   
        local vis = 3
        if cp1~=nil then 
            dan1 = cp1.danger
            vis = cp1.vision
        end
                
        local dan2 = dan1
        for i=1,vis do
            local cp2 = match:checkpoint(sp*(i+1)+carpos)        
            if cp2~=nil then
                if dan2>cp2.danger then dan2 = cp2.danger end
            end    
        end
                
        if false then
        elseif sp>dan2 then
            sup = -1        
        elseif sp<dan1*0.75 then
            sup = 2
        elseif sp<dan1*0.95 then
            sup = 1            
        elseif sp<dan1*1.05 then
            sup = 0
        else
            sup = -1
        end
        LOG:debug("Player","%s - %d,%d,%d",self.oid,sp,dan1,dan2)
    end
    LOG:debug("Player","%s speedUp(%d)",self.oid,sup)
    car:doSpeedUp(sup)
    return
end

-- << Class - Car >>
local Car = class.define("racing.car", {ge.daemon.Thing})
function Car:ctor()
    self:prop("pos", 0)
    self:prop("speed", 0) -- pos/1sec
    self:prop("maxSpeed", 100)
    self:prop("speedACC", 0) -- speed/1sec
    self:prop("levelACC", {-10,0,5,10})
end

function Car:enterMatrix()
    MT():obj("match"):add(self)
    self:startEngine()
end
function Car:startEngine()
    self:runtimeProp("engineEnable",true)        
    local interval = 100
    MT():delayTimer(0,interval, self.oid, "engine")    
end
function Car:stopEngine()    
    self:runtimeProp("engineEnable",false)
end
function Car:engine()
    if not V(self:runtimeProp("engineEnable"),false) then
        return false
    end
    
    local sp = self:prop("speed")
    if sp>0 then
        local npos = self:prop("pos") + sp*0.1
        MT():obj("match"):past(self, npos)
    end
    local spAcc = self:prop("speedACC")
    if spAcc~=0 then
        local nsp = self:prop("speed") + spAcc*0.1
        if nsp<0 then 
            nsp = 0
            self:changeProp("speedACC", 0)
            self:removeStatus("crack")
        end
        if nsp>self:prop("maxSpeed") then nsp = self:prop("maxSpeed") end
        self:changeProp("speed", nsp, "ACC")        
    end
end
function Car:doSpeedUp(level)
    if self:isBusy("speedUp") then return false end
    local al = self:prop("levelACC")
    self:changeProp("speedACC", al[level+2])
    return true
end

-- << Class - Match >>
local Match = class.define("racing.Match",{ge.daemon.MatrixObject})
Match.DANGER_PER_CAR = 0.25
Match.TRACK_TYPE = enum(
    {"START","END","NORMAL","STRAIGHT","CORNER"}
)
Match.CHECKPOINT_LEN = 100 -- >= MAX(maxSpeed)
local cp = function(pos)
    return math.floor(pos/Match.CHECKPOINT_LEN)
end
function Match:ctor()
    -- 跑道的checkpoint定义
    self.cars = {}
    self.track = {}
    
    self:defcp(0,1000,Match.TRACK_TYPE.STRAIGHT, 80 ,3)
    self:defcp(1000,1500,Match.TRACK_TYPE.NORMAL, 60 ,2)
    self:defcp(1500,1600,Match.TRACK_TYPE.CORNER, 40 ,1)
    self:defcp(1600,2500,Match.TRACK_TYPE.STRAIGHT, 80 ,3)
    self:defcp(2500,2500,Match.TRACK_TYPE.END, 80 ,1)
end
function Match:checkpoint(pos)
    local c = cp(pos)
    if c> #self.track then return self.track[#self.track] end
    return self.track[c]
end
function Match:defcp(pos1,pos2,type,danger,vision)
    local s = cp(pos1)
    local e = cp(pos2-1)
    if e<s then e=s end
    
    for i=s,e do
        local r = {
             type = type,
             danger = danger,
             vision = vision,
             cars = {},
        }
        table.insert(self.track,r)
    end
end
function Match:add(car)
    table.insert(self.cars, car)
    local ccp = cp(V(car:prop("pos"),0))
    if ccp >0 then
        table.insert(self.track[ccp].cars, car.oid)
    end
end
function Match:past(car, npos)
    car:changeProp("pos",npos)
    
    local ocp = V(car:runtimeProp("checkpoint"),0)
    local ncp = cp(npos)
        
    local cpinfo = self.track[ncp]
    if ocp~=ncp then        
        car:runtimeProp("checkpoint",ncp)
        
        local ocpinfo = self.track[ocp] 
        if ocpinfo~=nil then            
            table.walkRemove(ocpinfo.cars , function(k,v) return v==car.oid end)
        end
        if cpinfo~=nil then
            LOG:debug("Match","car[%s] reach checkpoint[%d - %s]",car.oid,ncp, Match.TRACK_TYPE.valueOf(cpinfo.type))
            table.insert(cpinfo.cars , car.oid)
        end
    end
    
    if cpinfo==nil then
        return
    end
    
    -- do checkpoint
    if cpinfo.type == Match.TRACK_TYPE.END then
        car:stopEngine()
        MT():obj(car:prop("driver")):stopHeartBeat()
        LOG:debug("Match","car[%s] reach end", car.oid)
    end    
    
    local sp = car:prop("speed")    
    local danger = cpinfo.danger/(1+(#cpinfo.cars-1)* Match.DANGER_PER_CAR)
    if not car:checkStatus("crack") and sp>danger then
        -- check crack?
        local accident = math.random(0,sp)
        LOG:debug("Match","check crack [%d/(%d,%d)",accident,sp, danger)
        if accident>=danger then
            print("!!!!! "..car.oid.." CRACK at "..npos.."!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
            -- fir event
            car:addStatus("crack",{},{"speedUp"})
            car:changeProp("speedACC", -((sp+1)/0.5))
            var_dump(car)
        end
    end
end

--<< RUN >>
local init = function(m)
    m.timeLabelFun = Matrix.TLF_SECOND
    m.config.Living.heartBeatTime = 1000    

    -- init statis object    
    local match = Match.new()
    m:enterObject("match", match)
end

if true then
    -- Living heart beat
    m  = Matrix.new()
    m:setup(0, 0, init)
    
    -- create runtime objects
    for i=1,1 do
        local car = Car.new()
        m:enterObject("car"..i, car)
        
        local player = Player.new()
        m:enterObject("player"..i, player)        
        player:drive(car)        
    end
    
    -- start
    m:begin()
        
    local rtime = 1000*100
    m:process(rtime)
    
    for _,car in ipairs(m:obj("match").cars) do
        print(car.oid, string.dump(car))
    end
        
    print("now", m:dumpTime())
    print("commands", "\n"..m:dumpCommand())
    
end

if false then
    print("\n---- save -----")
    
    local store = {}    
    m:save(Matrix.WRITER(store))
    var_dump(table.json(store))
end

-- ge/daemon/Thing.lua
-- 对应Matrix的逻辑存在物体，具有状态和命令行为的物体

require("bma.lang.Class")
require("bma.lang.ext.Table")
require("ge.daemon.Matrix")
require("ge.daemon.Command")
require("ge.daemon.State")

local Matrix = ge.daemon.Matrix
local MatrixObject = ge.daemon.MatrixObject
local BaseState = ge.daemon.state.BaseState

-- <<class - Thing>>
local LDEBUG = LOG:debugEnabled()
local LTAG = "Thing"
local Thing = class.define("ge.daemon.Thing",{MatrixObject, bma.lang.StdObject})

MatrixObject.defEvent("PropChange","Thing.propChange")
MatrixObject.defEvent("StateChange","Thing.stateChange")

Thing.defEvent = function(k,v)
    if Thing.EVENT[k] ~= nil then
        error("event["..k.."] exists")
    end
    Thing.EVENT[k] = v
end

function Thing:id(v)
    return self:attr("id",v)
end

function Thing:rootState(s)
    local st = self:runv("state")
    if st==nil and V(s,true) then 
        st = ge.daemon.state.MixState.new()
        st:name(V(self:id(),"")..".ROOT")
        self:runv("state",st)
    end
    return st
end

function Thing:changeProp(name,v,why)
    local old = self:prop(name)
    self:prop(name,v)    
    if old ~= v and why~=nil then
        local ev = {
            type=Thing.EVENT.PropChange,
            name = name,
            oldValue = old,
            value = v,
            why = why,
        }
        self:fireEvent(ev)
    end
end

function Thing:addProp(name,v,min,max,why)
    local old = V(self:prop(name),0)
    local val = old + v
    if min~=nil then
        local m1 = min
        if type(min)=="string" then
            m1 = self:prop(min)
        end
        if m1~=nil and val<m1 then
            val = m1
        end
    end
    if max~=nil then
        local m2 = min
        if type(max)=="string" then
            m2 = self:prop(max)
        end
        if m2~=nil and val>m2 then
            val = m2
        end
    end
    self:changeProp(name,val,why)
    return val,old
end

function Thing:pushState(stname, st)
    local rst = self:rootState()
    if st.bind ~= nil then
        st:bind(self)
    end
    rst:pushState(stname, st)
end

function Thing:popState(stname)
    local rst = self:rootState()    
    rst:popState(stname)
end

function Thing:isBusy(name)
    local rst = self:rootState(false)
    if rst~=nil then return rst:isBusy(name) end
    return false    
end

-- <<class - ObjectState>>
local ObjectState = class.define("ge.daemon.state.ObjectState",{BaseState, ge.daemon.command.CommandSource})

function ObjectState:bind(o)
    self:attr("me", o)
end

function ObjectState:me()
    return self:attr("me")
end

function ObjectState:onActive()
    local m = self:me()
    if m ~= nil then    
        if m~= nil then
            local ev = {
                type=Thing.EVENT.StateChange,
                state = self,
                add = true,
            }        
            m:fireEvent(ev)
        end
    end
    if self.activeCommand ~= nil then
        local cmd = self:activeCommand()
        if cmd:id()==nil then
            cmd:id(NEWCID(m))
        end
        if cmd~=nil then
            self:linkCommand(cmd)
        end
    end    
end

function ObjectState:onDeactive(remove)
    local m = self:me()
    if m~= nil then
        local ev = {
            type=Thing.EVENT.StateChange,
            state = self,
            remove = true,
        }        
        m:fireEvent(ev)
    end
    self:unlinkCommand()
    if remove then
        self:removeAttr("me")
    end
    self.temp = nil
end


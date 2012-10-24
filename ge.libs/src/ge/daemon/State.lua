-- ge/daemon/State.lua
-- 状态机

require("bma.lang.Class")
require("bma.lang.ext.Table")

local LDEBUG = LOG:debugEnabled()
local LTAG = "StateM"

-- <<class: BaseState>>
local BaseState = class.define("ge.daemon.state.BaseState",{bma.lang.StdObject})

function BaseState:id(v)
    return self:attr("id",v)
end

function BaseState:name(v)
    return self:attr("name",v)
end

function BaseState:parent(v)
    if v~=nil and not v then
        self:removeThisv("_parent")
    else
        return self:thisv("_parent",v)
    end    
end

function BaseState:root()
    local p = self:parent()
    if p==nil then return self end
    return p:root()
end

function BaseState:stateName(v)
    return self:runv("stateName", v)
end

function BaseState:walk(visitor)
    return visitor(self, true)
end

function BaseState:onActive()
    
end

function BaseState:onDeactive(remove)
    
end

function BaseState:isBusy(move)    
    local busy = self:attr("busy")   
    if busy ~= nil then             
        if type(busy)=="table" then
            for _,v in ipairs(busy) do
                if v==move then return true end
            end
        else
            return busy == move
        end
    end
    return false
end

function BaseState:setBusy(move)
    self:attr("busy",move)
end

function BaseState:addBusy(move)
    local bl = self:attr("busy")
    if bl==nil then
        self:attr("busy",move)
        return
    end
    
    if type(bl)~="table" then
        bl = {bl}
    end
    if type(move)=="table" then
        for _,v in ipairs(move) do
            table.insert(bl,v)
        end
    else
        table.insert(bl,move)
    end
end

-- <<class: StackState>>
local StackState = class.define("ge.daemon.state.StackState", {BaseState})

function StackState:ctor()
    self:runv("stack",{})
end

local _deactive = function(self,rem)
    local st = self:runv("active")
    if st ~= nil then
        if LDEBUG then
            LOG:debug(LTAG,"%s deactive(%s) - %s",V(self:name(),"?"), tostring(rem), V(st:name(),"?"))
        end
        st:onDeactive(rem)
        if rem then
            st:parent(false)
        end
        self:removeRunv("active")
    end
    return st
end

local _active = function(self, st)
    if st ~= nil then
        if LDEBUG then
            LOG:debug(LTAG,"%s active - %s",V(self:name(),"?"), V(st:name(),"?"))
        end
        self:runv("active", st)
        st:onActive()
    end
end

function StackState:isBusy(name)    
    if BaseState.isBusy(self, name) then return true end    
    local st = self:runv("active")
    if st ~= nil then        
        return st:isBusy(name)
    end
    return false
end

function StackState:pushState(st)    
    local ast = _deactive(self, false)
    if ast~= nil then        
        table.insert(self:runv("stack"), ast)
    end    
    st:parent(self)
    _active(self, st)
end

function StackState:popState()
    local ast = _deactive(self, true)
    if ast==nil then
        if LOG:warnEnabled() then
            LOG:warn(LTAG,"%s pop empty stack", V(self:name(),"?"))
        end
    end    
    local st = self:runv("stack")
    if #st>0 then
        local o = table.remove(st)
        _active(self,o)    
    end
end

function StackState:size()
    local st = self:runv("stack")
    local c = #st
    if self:runv("active") ~= nil then c = c+1 end
    return c
end

function StackState:walk(visitor)    
    local ast = self:runv("active")
    if ast == nil then
        return visitor(self, true)
    end
    visitor(self, false)
    return ast:walk(visitor)
end

-- <<class: GroupState>>
local GroupState = class.define("ge.daemon.state.GroupState", {BaseState})

function GroupState:children(s)
    local chl = self:attr("children")
    if chl==nil and V(s,false) then
        chl = self:attr("children",{})
    end
    return chl
end

local setState = function(self, k, nst)
    local chl = self:children(true)
    local st = chl[k]
    if st~=nil then
        if LDEBUG then
            LOG:debug(LTAG,"'%s' deactive(true) - %s:%s",V(self:name(),"?"),k, V(st:name(),"?"))
        end
        st:onDeactive(true)
        st:parent(false)
    end
    if nst~=nil then
        if LDEBUG then
            LOG:debug(LTAG,"'%s' active - %s:%s",V(self:name(),"?"), k, V(nst:name(),"?"))
        end
        nst:onActive()
    end    
    chl[k] = nst
    nst:parent(self)
    nst:stateName(k)    
end

function GroupState:walk(visitor)    
    local chl = self:children()
    local vs = false
    if chl ~= nil then
        for _,v in pairs(chl) do
            vs = true
            if not V(v:walk(visitor),true) then
                return false
            end
        end        
    end
    if vs then
        return true
    else
        return visitor(self, true)        
    end
end

function GroupState:isBusy(move)    
    if BaseState.isBusy(self,move) then return true end
    local chl = self:children()
    if chl~= nil then
        for _,v in pairs(chl) do
            if v:isBusy(move) then return true end
        end
    end
    return false
end

function GroupState:getState(name)
    local chl = self:children()
    if chl ~= nil then
        return chl[name]
    end
    return nil
end

function GroupState:hasState(name)
    return self:getState(name)~=nil
end

function GroupState:addState(st,stname)
    if st==nil then
        error("state is nil")
    end
    local name = stname
    if name==nil then name=st:name() end
    if name == nil then
        error("state name is nil")
    end
    setState(self,name,st)
end

function GroupState:removeState(stname)
    setState(self,name,nil)
end

-- <<class: MixState>>
local MixState = class.define("ge.daemon.state.MixState", {GroupState})

function MixState:pushState(stname, st)    
    local cst = self:getState(stname)
    if cst~=nil then
        cst:pushState(st)
        return
    end
    
    local sst = StackState.new()
    sst:name(stname)        
    self:addState(sst, stname)
    
    st:stateName(stname)
    sst:pushState(st)
end

function MixState:popState(stname)
    local cst = self:getState(stname)
    if cst~=nil then
        if cst:size()>0 then
            cst:popState()
        end
        return
    end    
end


-- ge/daemon/AI.lua
-- AI

require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Table")

-- class: Node
local Node = class.define("ge.daemon.ai.Node")

function Node:initState(hfsm, tags)
end

function Node:getNode(tag,level,err)
    return self
end

function BaseState:evaluate(ctx)
    return BaseState.EVAL.UNKNOW, 0
end

function BaseState:process(ctx)
    LOG:debug("BaseState","do nothing(%s)",string.dump(ctx))
end

function BaseState:onActive()
    
end

function BaseState:onDeactive(remove)
    
end

-- helper
function BaseState:pushTag(rtag,tag)
    if rtag == nil then
        rtag = {tag}
    else
        table.insert(rtag,1,tag)
    end
    return rtag
end

-- class: GroupState
local GroupState = class.define("ge.daemon.state.GroupState",{BaseState})

function GroupState:ctor()    
    self.children = {}
end

function GroupState:add(tag,ch)
    ch.tag = tag
    table.insert(self.children,ch)
end

function GroupState:initState(hfsm, tags)
    self:initChildrenState(hfsm, tags)
end

function GroupState:initChildrenState(hfsm, tags)
    if tags==nil then tags = {} end
    for _,ch in ipairs(self.children) do
        local ctags = table.clone(tags)
        table.insert(ctags, ch.tag)
        ch:initState(hfsm, ctags)
    end
end

function GroupState:getState(tag,level,err)
    if level>#tag then return self end
    local ctag = tag[level]
    
    for _,ch in ipairs(self.children) do
        if ch.tag == ctag then            
            return ch:getState(tag,level+1,err)
        end
    end
    if V(err,true) then
        error(string.format("state[%s] not exists",table.concat(tag,"/")))
    end
    return nil
end

function GroupState:evaluate(ctx)
    return self:evalChildren(ctx)
end

function GroupState:evalChildren(ctx)
    local elist = self:candidate(ctx)
    local rep = nil
    if #elist == 1 then
        rep = elist[1]
    elseif #elist>0 then
        rep = self:selectOne(elist)
    end
    if rep==nil then
        return BaseState.EVAL.UNKNOW,0
    end    
    return rep.eval,rep.weight,self:pushTag(rep.rtag,rep.ctag)
end

function GroupState:candidate(ctx)
    local elist = {}
    for _,ch in ipairs(self.children) do
        local e,w,rtag = ch:evaluate(ctx)        
        if e>=BaseState.EVAL.SELECT then
            local v = {eval=e,weight=w,ctag=ch.tag,rtag=rtag}
            if e==BaseState.EVAL.ACTIVE then 
                return {v}
            end
            table.insert(elist,v)
        end
    end    
    return elist
end

function GroupState:selectOne(elist)
    local mrep = nil
    for _,rep in ipairs(elist) do
        if mrep == nil then
            mrep = rep
        else
            if mrep.weight < rep.weight then
                mrep = rep
            end
        end
    end
    return mrep
end

-- class: WrapState
local WrapState = class.define("ge.daemon.state.WrapState", {BaseState})

function WrapState:bind(v)
    v.tag = "state"
    self.state = v
end

function WrapState:initState(hfsm, tags)    
    if self.state ~= nil then
        local ctags = {}
        if tags~=nil then table.copy(ctags, tags) end
        table.insert(ctags, self.state.tag)
        return self.state:initState(hfsm, ctags)
    end
end

function WrapState:getState(tag,level,err)
    if level>#tag then return self end
    if self.state ~= nil and tag[level] == self.state.tag then        
        return self.state:getState(tag,level+1,err)
    end
    if V(err,true) then
        error("invalid tag[%s]",tag[level])
    end
    return nil
end

function WrapState:evaluate(ctx)
    if self.state ~= nil then
        local e,w,rtag = self.state:evaluate(ctx)
        return e,w,self:pushTag(rtag,self.state.tag)    
    end
    return BaseState.EVAL.UNKNOW,0
end

function WrapState:process(ctx)
    if self.state ~= nil then
        return self.state:process(ctx)
    end
    return false
end

function WrapState:onActive()
    if self.state ~= nil then
        return self.state:onActive()
    end
end

function WrapState:onDeactive(remove)
    if self.state ~= nil then
        return self.state:onDeactive(remove)
    end
end

-- class: HFSM
local LDEBUG = LOG:debugEnabled()
local LTAG = "HFSM"

local HFSM = class.define("ge.daemon.HFSM", {GroupState})

function HFSM:ctor()
    self.stack = {}
end

function HFSM:getParent(level)
    if level<1 then return self:root() end    
    if self.parent == nil then return nil end
    if level==1 then return self.parent end
    return self.parent:getParent(level-1)
end

function HFSM:getRoot()
    if self.parent == nil then return self end
    return self.parent:getRoot()
end

function HFSM:initHFSM()
    self:initChildrenState(self)
end

function HFSM:initState(hfsm, tags)
    self.parent = hfsm
    self:initChildrenState(self, tags)
end

local _deactive = function(self)
    if self.active ~= nil then
        if LDEBUG then
            LOG:debug(LTAG,"deactive(%s,true)",table.concat(self.activeTag,"/"))
        end
        self.active:onDeactive(true)
        self.active = nil
        self.activeTag = nil
    end
end

local _active = function(self, state, tag)
    if state ~= nil then
        if LDEBUG then
            LOG:debug(LTAG,"active(%s)",table.concat(tag,"/"))
        end
        self.active = state
        self.activeTag = tag
        self.active:onActive()
    end
end

function HFSM:activeState(tag)
    local state = self:getState(tag,1)
    _deactive(self)
    _active(self,state,tag)
end

function HFSM:pushState(tag)
    local state = self:getState(tag,1)
    table.insert(self.stack,{state=self.active,tag=self.activeTag})
    _deactive(self)    
    _active(self,state,tag)    
end

function HFSM:popState()
    if self.active==nil then    
        error("stack empty,can't pop")
    end    
    _deactive(self)    
    if #self.stack>0 then
        local o = table.remove(self.stack)
        _active(self,o.state,o.tag)    
    end
end

function HFSM:size()
    return #self.stack
end

function HFSM:process(ctx)
    if self.active == nil then
        local e,w,rtag = self:evaluate(ctx)
        if e<BaseState.EVAL.SELECT then
            LOG:debug(LTAG,"evaluate fail")
            return false
        end
        if LDEBUG then
            LOG:debug(LTAG,"evaluate[%s]",table.concat(rtag,"/"))
        end
        self:activeState(rtag)
    end    
    local r,ntag = self.active:process(ctx)    
    local rt = type(r)
    if rt=="nil" then
        return nil
    end
    if rt=="boolean" then
        if r then
            return true
        else
            rt = "number"
            r = BaseState.RET.DEACTIVE
        end
    end
    if rt=="number" then        
        if r==BaseState.RET.DEACTIVE then
            self:popState()
            if self.active~=nil then 
                return false
            end
        elseif r==BaseState.RET.SWITCH then
            self:activeState(ntag) 
        end
        return true
    end
    return r,ntag
end

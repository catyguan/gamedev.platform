-- ge/arena/Character.lua

require("bma.lang.Class")
require("bma.lang.ext.Table")
require("ge.daemon.Matrix")
require("ge.daemon.Thing")

-- CA_xxxx / Character Attribute
CA_ID = "id"
CA_NAME = "name"
CA_LEVEL_GROUP = "levelGroup"
CA_LEVEL = "level"
CA_FACTION = "faction"
CA_CHAR_CLASS = "charClass"

-- << class - CharComponent >>
local CharComponent = class.define("ge.arena.CharComponent")

-- interface --
function CharComponent:attachCharacter(ch) end
function CharComponent:detachCharacter(ch) end
function CharComponent:charLevelup(ch,lvl,old) end

-- << class - Character >>
local Class = class.define("ge.arena.Character", {ge.daemon.Thing})

-- interface --
function Class:getCharViewPropNameList()
    return {}
end

-- interface --
function Class:onCharPropChange(n,v,diff)
    
end

-- interface end --
function Class:title()
    local id = self:id() or tostring(self)
    local name = self:attr(CA_NAME) or "??"
    return name.."#"..id
end

function Class:faction(v)
    return self:attr(CA_FACTION, v)
end

function Class:equipList(s)
    local l = self:attr("equip")
    if l==nil and s then l = self:attr("equip",{}) end
    return l
end

function Class:setEquip(v)
    local l = self:equipList(true)
    for i,e in ipairs(l) do
        if e:slot()==v:slot() then            
            l[i] = v
            e:detachCharacter(self)
            v:attachCharacter(self)
            return true
        end
    end
    table.insert(l, v)    
    v:attachCharacter(self)    
    return true
end

function Class:removeEquip(slot)
    local l = self:equipList()
    if l==nil then return false end
    for i,e in ipairs(l) do
        if e:slot()==slot then 
            table.remove(l,i)
            e:detachCharacter(self)
            return true
        end
    end
    return false
end

function Class:equip(slot)
    local l = self:equipList()
    if l==nil then return nil end
    for i,e in ipairs(l) do
        if e:slot()==slot then 
            return e
        end
    end
    return nil
end

function Class:addComponent(list, ob, f)
    for _,v in ipairs(list) do
        local match = false
        if f~=nil then 
            math = f(ob)
        else
            math = v:id()==ob:id()
        end 
        if math then return false end        
    end
    table.insert(list,ob)
    if ob.attachCharacter~=nil then
        ob:attachCharacter(self)
    end
    return true
end

function Class:removeComponent(list, f)
    for i,v in ipairs(list) do
        local match = false
        if type(f)=="function" then 
            match = f(ob)
        else
            match = v:id()==f
        end 
        if match then 
            table.remove(list,i)
            if v.detachCharacter~=nil then
                v:detachCharacter(self)
            end     
            return true
        end        
    end
    return false
end

function Class:skillList(s)
    local l = self:attr("skill")
    if l==nil and s then l = self:attr("skill",{}) end
    return l
end

function Class:addSkill(ob,f)
    local l = self:skillList(true)
    return self:addComponent(l,ob,f)
end

function Class:removeSkill(id)
    local l = self:skillList(true)
    return self:removeComponent(l,f)    
end

function Class:featList(s)
    local l = self:attr("feat")
    if l==nil and s then l = self:attr("feat",{}) end
    return l
end

function Class:addFeat(ob,f)
    local l = self:featList(true)
    return self:addComponent(l,ob,f)
end

function Class:removeFeat(id)
    local l = self:featList()
    return self:removeComponent(l,id)
end

function Class:effectList(s)
    local l = self:runv("effect")
    if l==nil and s then l = self:runv("effect",{}) end
    return l
end

function Class:addEffect(ob,f)
    local l = self:effectList(true)    
    return self:addComponent(l,ob,f)
end

function Class:removeEffect(id)
    local l = self:effectList()
    return self:removeComponent(l,id)
end

function Class:charClass(v)
    local r = self:attr(CA_CHAR_CLASS)
    if v~=nil then
        if r~= nil then
            r:detachCharacter(self)
        end
        r = self:attr(CA_CHAR_CLASS, v)
        v:attachCharacter(self)
    end
    return r
end

function Class:addCharProp(n,v)
    if v==0 then return v end    
    local nv,old = self:addProp(n,v)
    if nv==old then return nv end
    self:onCharPropChange(n,nv,nv-old)
    return nv,old
end

function Class:createProp(data)
        
end

function Class:create(data)
    local val = function(data,name)
        local v = data[name]
        if v==nil then error(name.." is nil") end
        return v
    end
    
    -- main    
    self:attr(CA_NAME,val(data,CA_NAME))    
    self:attr(CA_LEVEL_GROUP,V(data[CA_LEVEL_GROUP],"L"))
    self:attr(CA_FACTION,val(data,CA_FACTION))
    self:attr(CA_LEVEL,val(data,CA_LEVEL))    
        
    -- base
    self:createProp(data)
    
    local pplist = data.prop
    if pplist~=nil then
        for k,v in pairs(pplist) do
            self:addCharProp(k,v)
        end
    end
    
    -- class
    self:charClass(val(data,CA_CHAR_CLASS))
    
    -- equip    
    local eqlist = data.equip
    if eqlist ~= nil then
        for _,o in ipairs(eqlist) do
            self:setEquip(o)
        end        
    end
    
    -- skill
    local sklist = data.skill
    if sklist ~= nil then
        for _,o in ipairs(sklist) do             
            self:addSkill(o)
        end
    end
    
    -- feat
    local flist = data.skill
    if flist ~= nil then
        for _,o in ipairs(flist) do             
            self:addFeat(o)
        end
    end
    
    -- effect
    local eflist = data.effect
    if eflist ~= nil then
        for _,o in ipairs(eflist) do             
            self:addEffect(o)
        end
    end
    
end

function Class:level()
    return self:attr(CA_LEVEL)
end

function Class:levelup(lvl)
    local old = V(self:attr(CA_LEVEL),0)
    self:attr(CA_LEVEL, lvl)
    self:handleLevelup(lvl,old)
end

function Class:handleLevelup(lvl,old)
    local cls = self:charClass()
    if cls~=nil then
        cls:charLevelup(self, lvl, old)
    end
    
    local lfun = function(l)
        if l~=nil then
            for _,v in ipairs(l) do
                if v.charLevelup~=nil then
                    v:charLevelup(self, lvl, old)
                end
            end
        end
    end
    
    lfun(self:equipList())
    lfun(self:skillList())
    lfun(self:featList())
    lfun(self:effectList())
end

function Class:combatLevel()
    return self:attr(CA_LEVEL_GROUP)..self:attr(CA_LEVEL)
end

function Class:viewProp(n)
    return self:prop(n)
end

function Class:view(r)
    if r==nil then r = {} end
    local list = self:getCharViewPropNameList()
    for i,n in ipairs(list) do
        local v = self:viewProp(n)
        if v~=nil then
            r[n] = v
        end
    end
    return r
end

function Class:diff(view, r)
    if r==nil then r = {} end
    local list = self:getCharViewPropNameList()
    for i,n in ipairs(list) do
        local v1 = self:viewProp(n)
        local v2 = view[n]
        if v1~=v2 then
            local v = {me=v1,ob=v2}
            if type(v1)=="number" or type(v2)=="number" then
                v.diff = V(v1,0)-V(v2,0)
            end
            r[n] = v
        end
    end
    return r
end

function Class:tryClass(newClass)
    local old = self:charClass()
    local view = self:view()
    self:charClass(newClass)
    local r = self:diff(view)
    self:charClass(old)    
    return r
end

function Class:tryEquip(slot, o)
    if slot==nil then slot=o:slot() end
    local old = self:equip(slot)
    local view = self:view()
    if o~=nil then
        self:setEquip(o)
    else
        self:removeEquip(slot)
    end
    local r = self:diff(view)
    if old~=nil then
        self:setEquip(old)
    else
        self:removeEquip(slot)    
    end
    return r
end

function Class:isDie()
    return V(self:prop(CP_HP),0)<=0
end
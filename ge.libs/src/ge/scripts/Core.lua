-- ge/scripts/Core.lua

require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Table")

local LDEBUG = LOG:debugEnabled()
local LTAG = "Scripts"

-- macro

--<< struct - Token>>
--[[
type:int - (-1:END)(1:WORD)(2:;)(3:=)(4:TAB)(5:NEWLINE)(6:SPACE)
token:string
line:int
pos:int
]]
--<< struct - ParserNode>>
--[[
name:string
p:[{name:string,value:string,line:int,pos:int}..]
line:int
pos:int
]]

-- << interface - Reader >>
--[[
function nextByte():int
    return -1 mean end
]]
local StringReader = class.define("ge.scripts.StringReader",{})
function StringReader:ctor(s)
    self.data = s
    self.pos = 0
end
function StringReader:nextByte()
    if self.pos<#self.data then
        self.pos = self.pos+1
        return self.data:byte(self.pos)
    end
    return -1
end
function StringReader.create(s)
    local r = StringReader.new(s)
    return function()
        return r:nextByte()
    end
end

-- << class - ScriptParser >>
local ScriptParser = class.define("ge.scripts.ScriptParser",{})

function ScriptParser:ctor()
    self.line = 1
    self.lastLine = 1
    self.pos = 1
    self.lastPos = -1
    self.lastByte = -2
end

function ScriptParser:setReader(r)
    self.reader = r
end

function ScriptParser:token(t,k)
    return {
        type=t,
        token=k,
        line = self.lastLine,
        pos = self.lastPos,
    }
end

function ScriptParser:readByte()
    if self.lastByte==-1 then return -1 end
    local r = self.lastByte
    
    if r~=-2 then                
        self.lastByte = -2
    else
        r = self.reader()
        while r== 13 do
            r = self.reader()
        end
        if r==-1 then self.lastByte = -1 end
    end
    
    self.lastLine = self.line
    self.lastPos = self.pos
    if r==10 then
        self.line = self.line + 1
        self.pos = 0
    end
    self.pos = self.pos + 1
        
    return r
end

function ScriptParser:unreadByte(b)
    self.lastByte = b
    self.line = self.lastLine
    self.pos = self.lastPos    
end

local checkByte = function(b,f)
    if b==-1 then return false end
    
    local t = type(f)
    if t=="function" then
        return f(b)    
    end
    if t=="table" then
        for _,v in ipairs(f) do
            if b==v then return true end
        end
        return false
    end
    return f==b
end

function ScriptParser:readByteFor(f)
    local b = self:readByte()
    local r = ''
    while checkByte(b,f) do
        r = r .. string.char(b)
        b = self:readByte()
    end
    self:unreadByte(b)
    return r, b
end

local SPchecker = function(b)
    if b==32 then return true end
    if b==44 then return true end
    if b==59 then return true end
    return false
end

local EQchecker = function(b)
    if b==61 then return true end
    if b==58 then return true end
    return false
end

local ReadWord = function(self,b)
    local word = ""
    local quote = 0
    if b==34 or b==39 then  -- " '
        quote = b
        b = self:readByte()
    end
    
    while true do
        local ew = false
        if b==-1 or b==10 then  -- \n
            ew = true
        elseif quote>0 then
            if b==quote then
                ew = true
                b = self:readByte()
            elseif quote==34 then -- "
                if b==92 then  -- \
                    local b2 = self:readByte()
                    if b2==92 then 
                        b = 92 
                    elseif b2==110 then 
                        b = 10 
                    elseif b2==116 then 
                        b = 9 
                    else
                        b = b2
                    end
                end
            end
        else
            if b==32 or SPchecker(b) or EQchecker(b) then  -- spaces
                ew = true
            end
        end
        if ew then
            self:unreadByte(b)
            return word
        end
        word = word .. string.char(b)
        b = self:readByte()
    end
end

function ScriptParser:readToken()
    if self.lastToken~=nil and #self.lastToken>0 then
        return table.remove(self.lastToken)
    end
    local b = self:readByte()
    
    if b==-1 then return self:token(-1) end
    if b==9 then                
        local tab = string.char(9)..self:readByteFor(9)        
        return self:token(4,#tab)
    end
    if b==10 then return self:token(5,"\\n") end
        
    if b==32 then
        local tmp, b2 = self:readByteFor(32)
        if SPchecker(b2) then
            b = self:readByte()
            self:readByteFor(32)
            return self:token(2, string.char(b))
        elseif EQchecker(b2) then
            b = self:readByte()
            self:readByteFor(32)
            return self:token(3, string.char(b))
        else
            return self:token(6,#tmp+1);
        end
    end    
    
    if SPchecker(b) then
       self:readByteFor(32)
       return self:token(2, string.char(b))
    end            
    
    if EQchecker(b) then
       self:readByteFor(32)
       return self:token(3, string.char(b))
    end
    
    -- word    
    local tk = self:token(1,nil)    
    tk.token = ReadWord(self,b)
    return tk
end

function ScriptParser:unreadToken(t)
    if self.lastToken==nil then self.lastToken = {} end
    table.insert(self.lastToken, t)
end

local ParseNode
local ParseAttribute
local ComitAttribute = function(self,node,k,v,tk)
    if node.p == nil then node.p = {} end
    table.insert(node.p,{attr=true,name=k,value=v,line=tk.line,pos=tk.pos})
end

ParseAttribute = function(self,node)
    -- type:int - (-1:END)(1:WORD)(2:;)(3:=)(4:TAB)(5:NEWLINE)(6:SPACE)
    while true do
        local tk = self:readToken()
        if tk.type==-1 then
            self:unreadToken(tk)
            return false
        end
        if tk.type==5 then
            return true
        end
        
        if tk.type==2 or tk.type==3 or tk.type==4 or tk.type==6 then
            -- skip
        else        
            local tk2 = self:readToken()
            while tk2.type==4 or tk2.type==6 do
                tk2 = self:readToken()
            end
            if tk2.type==3 then
                local tk3 = self:readToken()
                while tk3.type==4 or tk3.type==6 do
                    tk3 = self:readToken()
                end 
                if tk3.type==1 then                
                    ComitAttribute(self,node,tk.token,tk3.token,tk)
                else
                    self:unreadToken(tk3)
                    ComitAttribute(self,node,tk.token,"",tk)    
                end
            else
                self:unreadToken(tk2)
                ComitAttribute(self,node,"",tk.token,tk)
            end
        end 
    end
end

ParseNode = function(self, stack)
    -- type:int - (-1:END)(1:WORD)(2:;)(3:=)(4:TAB)(5:NEWLINE)(6:SPACE)
    local tk = self:readToken()
    if tk.type==-1 then
        self:unreadToken(tk)
        return false
    end
    if tk.type==5 then
        return true
    end
    
    -- tab?
    local tab = 0
    local tk2
    if tk.type==4 then
        tab = tk.token        
    elseif tk.type==6 then
        tab = math.ceil(tk.token/4)
    else
        tk2 = tk
    end
    
    local cur
    local level
    while #stack>0 do
        local n = table.remove(stack)       
        if n.l<=tab then
            cur = n.n
            level = n.l
            table.insert(stack,n)
            break
        end            
    end    
    
    if tk2==nil then tk2 = self:readToken() end
    if tk2.type==1 then
        local tk3 = self:readToken()
        if tk3.type==3 then -- WORD=
            self:unreadToken(tk3)            
            self:unreadToken(tk2)            
            return ParseAttribute(self, cur)
        end        
        
        local node = {
            name=tk2.token,line=tk2.line,pos=tk2.pos
        }        
        if cur.p == nil then cur.p = {} end
        table.insert(cur.p, node)
        table.insert(stack,{n=node,l=level+1})
        
        return ParseAttribute(self, node)
    else
        self:unreadToken(tk2)        
        return ParseAttribute(self, cur)
    end
end

function ScriptParser:parse()
  local root = {
    name='ROOT',line=0,pos=0
  }
  local stack = {
    {n=root,l=0}
  }
  while ParseNode(self, stack) do end
  return root
end

function ScriptParser:rewrite(node,level)
    local r = ""
    level = level or 0
    if node.p ~= nil then
        for _,v in ipairs(node.p) do
            local tab = string.rep(' ',level*4)            
            if v.attr then
                r = r .. tab
                if v.name~=nil and v.name ~= "" then
                    r = r .. v.name
                end
                r = r  .. "=" .. v.value
                r = r .. "\n"
            else
                r = r .. tab .. v.name .. "\n"            
                r = r .. self:rewrite(v,level+1)
            end                
        end
    end
    return r
end

-- << class - ScriptGenerate >>
local ScriptGenerate = class.define("ge.scripts.ScriptGenerate",{})

function ScriptGenerate:ctor()
    self.actions = {}
end

function ScriptGenerate:setAction(names, action)
    if type(names)=="table" then
        for _,v in ipairs(names) do
            self.actions[v] = action
        end
    else
        self.actions[names] = action
    end
end

function ScriptGenerate:getAction(name)
    return self.actions[name]
end

function ScriptGenerate:check(node, results)
    if node.name=="ROOT" then
        return self:checkChildren(node, results)
    end
    
    local action = self:getAction(node.name)
    if action==nil then
        table.insert(results, {err="unknow action",message=node.name,line=node.line,pos=node.pos})
        return false
    else
        return action:check(self, node, results)
    end    
end

function ScriptGenerate:checkChildren(node, results)
    if node.p~=nil then
        for _,n in ipairs(node.p) do
            if not n.attr then
                self:check(n, results)
            end                
        end
    end
end

function ScriptGenerate:generate(node, meta, results)
    if node.name=="ROOT" then
        return self:generateList(node.p, meta, results)
    end
    
    local action = self:getAction(node.name)
    if action==nil then
        table.insert(results, {err="unknow action",message=node.name,line=node.line,pos=node.pos})
        return nil
    else
        return action:generate(self, node, meta, results)
    end
end

function ScriptGenerate:generateList(list, meta, results)
    if list==nil then return nil end
    
    local r = {}
    for _,n in ipairs(list) do
        if not n.attr then
            local o = self:generate(n,  meta, results)
            if o~=nil then table.insert(r, o) end
        end
    end
    return r    
end

require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Json")

testcase.include("bma.lang.ext.IO")
testcase.include("ge.scripts.Core")

if false then
    -- reader
    if false then
        local r = ge.scripts.StringReader.create("abcd广州\n\rbcde，")
        local ch = r()
        while ch~=-1 do
            print(ch, ch>0x80)
            ch = r()
        end
    end
    
    -- token reader
    print("parser reader")
    
    local r = ge.scripts.StringReader.create("abcd广州\n\rbcde，")
    local p = ge.scripts.ScriptParser.new()
    p:setReader(r)
    if false then
        local ch = p:readByte()
        while ch~=-1 do
            print(ch,p.lastLine,p.lastPos)
            ch = p:readByte()
        end
    end
    if true then
        print(p:readByteFor(function(b)return b~=10 end))
    end
end

local path = "d:/workroom/cocos2d-x/gamdev.myguild/ge.libs/test/ge/scripts/"
local str1 = io.readContent(path.."demo1.txt")
local str2 = io.readContent(path.."demo2.txt")
-- print(str)

if false then
    -- token --
    local r = ge.scripts.StringReader.create(str1)
    local p = ge.scripts.ScriptParser.new()
    p:setReader(r)
    
    local tk = p:readToken()
    local i = 0
    while tk~=nil and tk.type~=-1 and i<20000 do
        print(var_dump(tk))
        tk = p:readToken()
        i = i + 1        
    end
end

if true then
    -- parse & rewrite --
    local r = ge.scripts.StringReader.create(str2)
    local p = ge.scripts.ScriptParser.new()
    p:setReader(r)
    
    local r = p:parse()    
    print(p:rewrite(r))
end

if true then
    -- check --
    local r = ge.scripts.StringReader.create(str2)
    local p = ge.scripts.ScriptParser.new()
    p:setReader(r)
    
    local r = p:parse()    
    
    local g = ge.scripts.ScriptGenerate.new()
    local rlist = {}
    g:check(r, rlist)
    for _,v in ipairs(rlist) do
        var_dump(v)
    end        
    print("done")
end

if false then
    -- generate --
    local r = ge.scripts.StringReader.create(str2)
    local p = ge.scripts.ScriptParser.new()
    p:setReader(r)
    
    local r = p:parse()    
    
    local g = ge.scripts.ScriptGenerate.new()
    local rlist = {}
    g:check(r, rlist)
    for _,v in ipairs(rlist) do
        var_dump(v)
    end        
    print("done")
end
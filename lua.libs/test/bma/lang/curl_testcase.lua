require("bma.lang.ext.Core")

-- curl
if false then
    local curl = require("curl")
    print(curl.escape("abcd$%^&*()"))
    local function get_html(url, c)
        local result = { }
        if c == nil then 
            c = curl.new() 
        end
        c:setopt(curl.OPT_URL, url)
        c:setopt(curl.OPT_WRITEDATA, result)
        c:setopt(curl.OPT_WRITEFUNCTION, function(tab, buffer)
            table.insert(tab, buffer)
            return #buffer
        end)
        local ok = c:perform()
        return ok, table.concat(result)
    end
    
    local ok, html = get_html("http://cn.bing.com/")
    if ok then
        print(string.format("get_html => " .. #html))
    else
        print "get_html fail"
    end
end    

-- httpRequest
testcase.include("bma.lang.HttpRequest")
local HttpRequest = bma.lang.HttpRequest 
local url = "http://live.yy.com/g/dump.php"
if true then    
    local content,hr = HttpRequest.getContent(url)
    print("content:"..content)
    print("result:"..hr:toString(100))
end

if true then    
    local content,hr = HttpRequest.getContent(url,{a=1})
    print("content:"..content)
    print("result:"..hr:toString(100))
end

if true then    
    local content,hr = HttpRequest.postContent(url,{a=1})
    print("content:"..content)
    print("result:"..hr:toString(100))
end
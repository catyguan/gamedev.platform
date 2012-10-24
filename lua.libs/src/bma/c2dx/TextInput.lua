-- bma/c2dx/TextInput.lua

module(..., package.seeall)

function textInput(fun, placeholder, fontName, fontSize, textLimit, dimensions, alignment)
    local f = fun
    if textLimit ~= nil then
        f = function(obj)                    
            if obj:getCharCount()>textLimit then
                local s = obj.last
                if s==nil then s = "" end
                obj:setString(s)
            else
                obj.last = obj:getString()
            end            
            fun(obj)
        end
    end    
    
    print(placeholder, fontName, fontSize)
    local tf
    if dimensions == nil then
        tf = CCTextInput:create(f,placeholder,fontName,fontSize)
    else
        tf = CCTextInput:create(f,placeholder,dimensions,alignment,fontName,fontSize)
    end
    
    return tf
end
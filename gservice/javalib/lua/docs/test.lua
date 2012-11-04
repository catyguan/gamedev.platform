function printall(...)
    local json = require("cjson")
    print(json.encode({...}))
end

function mulret()
    local t1 = {k1="v1",k2=123,k3=true}
    local t2 = {true,t1}
    return true,100,nil,24.3,"bye",t2
end

function error_fun()
    error("error test")
end

print("hi,i am in lua,中文")
print("env = ",env)
return true
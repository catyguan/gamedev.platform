print(java.call)
print("javaBack:")
local done,err,et = pcall(function()
    print(java.call("test","test2"))
    print(java.call("test","test3"))
end)
if not done then
    print("error",err)
    print(et)
end
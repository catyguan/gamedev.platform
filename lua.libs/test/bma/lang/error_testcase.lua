-- 错误处理
function foo()
    error("error0")
end

print("----------here is error test-----------")
local done,err,trace = pcall(foo)
if not done then
    print(err)
    print(trace)
end


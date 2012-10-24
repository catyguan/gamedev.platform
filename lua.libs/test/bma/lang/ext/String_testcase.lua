---- string
require("bma.lang.ext.Core")

testcase.include("bma.lang.ext.String")
testcase.include("bma.lang.ext.Dump")

if false then
	-- hex2bin
	local s1 = "32333435"
	local b1 = s1:hex2bin()
	print(b1)

	-- bin2hex
	print(b1:bin2hex())

	-- trim
	local s2 = "  abcded  \n"
	print(s2:trim())

	-- split
	local s3 = "a,b,c,d"
	local r3 = s3:split(",")
	print(string.dump(r3))
end

if true then
	local s1 = "abcd"
	local s2 = "a.b.c.d"
	print(s1, "caps",s1:caps())
	print(s2, "pad",s1:pad(10,"0"))

end
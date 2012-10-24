-- testcase
require("bma.lang.ext.Core")

if true then
	testcase.include("bma.lang.ext.MD5")

	local s = "hello world中文"
	print(s,"MD5",s:md5())
end

if true then
	testcase.include("bma.lang.ext.DES56")

	local key = "12345678"
	local s = "hello world"
	local v = s:crypt(key)
	print(v,"DES56",v:decrypt(key))
end

-- php
-- $str = pack('H*', $str);
-- $text = mcrypt_decrypt(MCRYPT_DES, $key, $str, MCRYPT_MODE_ECB);
-- 未处理pad问题
require("bma.test.bootstrap")

local S
if true then
	S = class.forName("bma.http.client.host.Service")
	S.install()
end	

local hc = class.instance("bma.http.client.Service")

if true then
	print("base:")
	local url = "http://localhost:8080/common.test/dump.php"
	local headers = {}
	headers["Accept-Charset"] = "utf-8"
	local params = {}
	params["q"] = "中文";
	
	local cb = function(err, r)
		print(err,r)
	end	
	hc:execute(cb, url, headers, params)
end

if false then
	print("post:")
	-- TODO
	print("\n")
end

if false then
	print("redirect:")
	-- TODO
	print("\n")
end

if false then
	print("timeout:")
	local url = "http://localhost:8080/common.test/dump.php"	
	-- $h1->setTimeout(5)
	-- $r = $h1->invoke()
	-- var_dump($r)	
end

if false then
	print("exception:")
	-- TODO
	print("\n")
end
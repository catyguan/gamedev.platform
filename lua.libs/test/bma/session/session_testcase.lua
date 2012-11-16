-- require("bma.test.bootstrap")
require("bma.lang.ext.Dump")

if false then
	local SI = class.forName("bma.session.local.Service")
	SI.install()
end	

local S = class.instance("bma.session.Service")

-- create/get
if true then

	local sess
	local f = function(err, s)
		sess = s
	end
	S:getSession(f,nil,true)
	var_dump(sess)
	
	print(sess:isTimeout(60))
end

-- timeout
if true then

	local sess
	local f = function(err, s)
		sess = s
	end
	S:getSession(f,nil,true)
	var_dump(sess)
	
	print(sess:isTimeout(60))
end

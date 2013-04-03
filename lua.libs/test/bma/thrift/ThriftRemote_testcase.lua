require("bma.lang.ext.Dump")

include("bma.thrift.RemoteService")

if true then
	local HS = class.forName("bma.thrift.RemoteService")
	HS.install()
end

local HS =  class.instance("bma.thrift.RemoteService")

if true then
	print("testcast start");	
	if not HS:isOpen() then
		print("remote not open, open it...")
		HS:open()
	else
		print("send message")
		local msg = {
			title = "Dr.Hello"
		}
		local seqid= HS:call("hello.name", msg, 5000, function(v) var_dump(v) end)
		print("seqid = ",seqid)
	end
	
end
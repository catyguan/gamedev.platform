-- bma/http/client/host/Service.lua
require("bma.lang.ext.Core")
require("bma.http.client.BaseService")

local Class = class.define("bma.http.client.host.Service", {bma.http.client.BaseService})

function Class.install()
	local o = Class.new()
	class.setInstance("bma.http.client.Service", o)
end

function Class:ctor()
	
end

--[[
{
	callback:AICalLStack, 
	url:string, 
	headers:map<name,value>, 
	params:map<name,value>, 
	content:string, 
	opts:map<string,value>
}
return {status:int,headers:map<name,value>,content:string}
]]
function Class:execute(callback, url, headers, params, content, opts)
	local HS = class.instance("bma.host.Service")
    return HS:call(callback, "http.client.execute", url, headers, params, opts)
end
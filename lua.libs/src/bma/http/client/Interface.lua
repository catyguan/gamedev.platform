-- bma/http/client/Interface.lua

--[[
class bma.http.client.Service
{
	function execute(
		callback, 
		url:string, 
		headers:map<name,value>, 
		params:map<name,value>, 
		content:string, 
		opts:map<string,value>
	) : {status:int,headers:map<name,value>,content:string}
}
options {
    
}
]] 
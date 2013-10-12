-- bma/http/client/HttpResponse.lua
local Class = class.define("bma.http.client.HttpResponse")

function Class:ctor(o)	
	if o==nil then o = EMPTY_TABLE end
	self.data = o	
end

function Class:hasResponse()
	return V(self.data.result, false)
end

function Class:getHeader(name)
	local n = string.lower(name)
	if self.data.headers~=nil then
		local l = #self.data.headers
		for i=1,l,1 do
			local v = self.data.headers[i]
			if string.lower(v.name)==n then
				return v.value
			end
		end
	end
	return nil
end

function Class:getHeaders(name)
	local n = string.lower(name);
	local r = {};
	if self.data.headers~=nil then
		local l = #self.data.headers
		for i=1,l,1 do
			local v = self.data.headers[i]
			if string.lower(v.name)==n then
				table.insert(r,v.value)
			end
		end
	end
	return r
end

function Class:getCookies()
	return self:getHeaders("Set-Cookie");
end

function Class:get200Content()
	if V(self.data.status, 0)~=200 then
		return ""
	end
	return self.data.content
end

function Class:get200JsonData()
	local r = self:get200Content()
	if r~="" then
		require("bma.lang.ext.Json")
		return r:json()
	end
	return nil
end

function Class:toString(contentLen)
	local d = "<nil>"
	local len = 0
	if self.data.content~=nil then
		d = self.data.content
		len = #self.data.content
	end
	if len>contentLen then
		d = string.sub(d,1,contentLen)
	end

	local tmp = "nil"
	if self.data.url~=nil then tmp = self.data.url end
	return string.format("httpclient.response(%d)(%s)[%d:%d:%d] => %s", V(self.data.id,0),tmp,V(self.data.errno, 0),V(self.data.status, 0),len,d)
end
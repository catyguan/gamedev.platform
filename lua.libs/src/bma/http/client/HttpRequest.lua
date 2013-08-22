-- bma/http/client/HttpRequest.lua
local Class = class.define("bma.http.client.HttpRequest")

function Class:ctor()
	self.postMode = false
	self.redirectMode = false
	self.exceptionMode = false
	self.timeout = 0
	-- self.headers	
end

function Class:setUserAgent(value)
	self:setHeader("User-Agent", value)
end

function Class:setHost(value)
	self:setHeader("Host", value)
end

function Class:setCookie(name, value)
	local ck = self:getHeader("Cookie")
	if ck==nil then 
		ck = {}
		self:setHeader("Cookie", ck)
	end
	ck[name] = value	
end

function Class:setReferrer(value)
	self:setHeader("Referrer", value)
end

function Class:setHeader(name, value)
	if self.headers==nil then
		self.headers = {};
	end
	self.headers[name] = value;
end

function Class:getHeader(name)
	if self.headers~=nil and self.headers[name] ~= nil then
		return self.headers[name];
	end
	return null;
end

function Class:removeHeader(name)
	if self.headers ~=null and  self.headers[name] ~= nil then
		self.headers[name] = nil
	end
end

function Class:setParam(name,value)
	if self.params == nil then
		self.params = {}
	end
	self.params[name] = value;
end

function Class:getParam(name)
	if self.params ~= nil and self.params[name] ~= nil then
		return self.params[name]
	end
	return null;
end

function Class:removeParam(name)
	if self.params ~=nil and  self.params[name] ~= nil then
		self.params[name] = nil
	end
end

function Class:buildQuery(t)
	if type(t) == "table" then
		data = ""
		for k,v in pairs(t) do
			if type(v)=="table" then
				for i,vv in ipairs(v) do
					if data ~="" then data = data..'&' end
					data = data.. modCurl.escape(k).."="..modCurl.escape(v)
				end
			else
				if data ~="" then data = data..'&' end
				data = data.. modCurl.escape(k).."="..modCurl.escape(v)
			end
		end
		return data
	else
		return t
	end
end

function Class:create()
	local r = {}	
	r.url = self.url
	r.params = self.params
	r.postfield = self.postfield
	r.timeout = self.timeout
	r.postMode = self.postMode
	r.redirectMode = self.redirectMode
	r.timeout = self.timeout
	r.headers = self.headers
	r.debug = self.debug
	return r
end

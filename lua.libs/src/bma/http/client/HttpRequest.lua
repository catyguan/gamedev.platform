-- bma/http/client/HttpRequest.lua

require("bma.lang.ext.Core")
local modCurl = require("curl")

module(..., package.seeall)

Result = class();

function Result:ctor()
	self.errno = 0
	self.status = 0
	self.time = 0
end

function Result:getHeader(name)
	local n = string.lower(name)
	if self.headers~=nil then
		local l = #self.headers
		for i=1,l,1 do
			local v = self.headers[i]
			if string.lower(v.name)==n then
				return v.value
			end
		end
	end
	return nil
end

function Result:getHeaders(name)
	local n = string.lower(name);
	local r = {};
	if self.headers~=nil then
		local l = #self.headers
		for i=1,l,1 do
			local v = self.headers[i]
			if string.lower(v.name)==n then
				table.insert(r,v.value)
			end
		end
	end
	return r
end

function Result:getCookies()
	return self:getHeaders("Set-Cookie");
end

function Result:get200Content()
	if self.status~=200 then
		return ""
	end
	return self.content
end

function Result:toString(contentLen)

	local d = "<nil>"
	local len = 0
	if self.content~=nil then
		d = self.content
		len = #self.content
	end
	if len>contentLen then
		d = string.sub(d,1,contentLen)
	end

	local tmp = "nil"
	if self.url~=nil then tmp = self.url end
	return string.format("HttpRequest.Result(%s)[%d:%d:%d] => %s", tmp,self.errno,self.status,len,d)
end

Curl= class();

function Curl:ctor()
	self.postMode = false
	self.redirectMode = false
	self.exceptionMode = false
	self.timeout = 5
	self._rheaders = {}
	self._content = {}
end

function Curl:setHeader(name,value)
	if self.headers==nil then
		self.headers = {};
	end
	self.headers[name] = value;
end

function Curl:getHeader(name)
	if self.headers~=nil and self.headers[name] ~= nil then
		return self.headers[name];
	end
	return null;
end

function Curl:removeHeader(name)
	if self.headers ~=null and  self.headers[name] ~= nil then
		self.headers[name] = nil
	end
end

function Curl:setParam(name,value)
	if self.params == nil then
		self.params = {}
	end
	self.params[name] = value;
end

function Curl:getParam(name)
	if self.params ~= nil and self.params[name] ~= nil then
		return self.params[name]
	end
	return null;
end

function Curl:removeParam(name)
	if self.params ~=nil and  self.params[name] ~= nil then
		self.params[name] = nil
	end
end

function buildQuery(t)
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

function Curl:create()
	local ps = "";
	if self.params ~= nil then
		ps = buildQuery(self.params)
	end

	local rurl = self.url
	if ps ~= "" and not self.postMode then
		local prefix  = "?"
		if string.find(self.url, "?") ~= nil then
			prefix = '&';
		end
		rurl = self.url .. prefix .. ps;
	end

	local ch = modCurl.new();

	local writeFun = function(th, buffer)
		table.insert(th._content, buffer)
		return #buffer
	end
	ch:setopt(modCurl.OPT_WRITEDATA, self)
	ch:setopt(modCurl.OPT_WRITEFUNCTION, writeFun)

	if self.timeout>0 then
		local vtimeout =  self.timeout
		if self.timeout<1 then vtimeout = 1 end
		ch:setopt(modCurl.OPT_TIMEOUT,vtimeout)
	end
	ch:setopt(modCurl.OPT_URL, rurl)
	if self.redirectMode then
		ch:setopt(modCurl.OPT_FOLLOWLOCATION, true)
		ch:setopt(modCurl.OPT_MAXREDIRS, 5)
	end

	local ths = {}
	if self.headers ~= nil then
		for k,v in pairs(self.headers) do
			table.insert(ths,k..": "..v)
		end
		ch:setopt(modCurl.OPT_HTTPHEADER,table.concat(ths,"\n\r"))
	end

	local headerFun = function(th, buffer)
		-- read header
		require("bma.lang.String")
		local a = buffer:split(":")
		if #a >1 then
			local n = table.remove(a,1)
			local v = table.concat(a,":")
			table.insert(th._rheaders,{
				name = n:trim(),
				value = v:trim(),
			})
		end
		return #buffer
	end
	ch:setopt(modCurl.OPT_HEADERDATA,self)
	ch:setopt(modCurl.OPT_HEADERFUNCTION, headerFun)

	local data = ""
	if self.postMode or self.postfield ~= nil then
		ch:setopt(modCurl.OPT_POST, true)
		if self.postfield~=nil then
			data = buildQuery(self.postfield)
		else
			data = ps
		end
		ch:setopt(modCurl.OPT_POSTFIELDS, data)
	end
	return ch
end

function Curl:invoke()
	local time_start = os.mclock()
	local ch = self:create()
	self._rheaders = {}
	self._content = {}
	local ok,msg,errno = ch:perform()
	local time_end = os.mclock()
	local r = Result.new()
	r.url = ch:getinfo(modCurl.INFO_EFFECTIVE_URL)
	r.time = time_end-time_start
	if ok then
		r.content = table.concat(self._content)
		r.status = ch:getinfo(modCurl.INFO_RESPONSE_CODE)
		r.headers = self._rheaders
	else
		r.errno = errno
		r.content = msg
	end
	return r
end

function getContent(url,params, timeout)
	if type(params) == "number" then
		timeout = params
		params = nil
	end
	if timeout == nil then
		timeout = 5
	end

	local hreq = Curl.new()
	hreq.url = url
	if params~=nil then
		hreq.params = params
	end
	hreq.timeout = timeout
	hreq:setHeader("User-Agent","MGR/httprequest")
	local r = hreq:invoke()
	return r:get200Content(), r
end

function postContent(url,data,timeout)
	if timeout == nil then
		timeout = 5
	end

	local hreq = Curl.new()
	hreq.url = url
	hreq.postfield = data
	hreq.timeout = timeout
	hreq:setHeader("User-Agent","MGR/httprequest")
	local r = hreq:invoke()
	return r:get200Content(), r
end
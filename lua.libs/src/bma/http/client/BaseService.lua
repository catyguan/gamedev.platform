-- bma/http/client/BaseService.lua
require("bma.lang.ext.Core")

local Class = class.define("bma.http.client.BaseService")

function Class:ctor()
	
end

function Class:addListener(f, id)
	if not self._listeners then
		self._listeners = {}
	end
	table.insert( self._listeners, {f=f,id=id} )
end

function Class:removeListener(id)
	if self._listeners then
		for i, o in ipairs(self._listeners) do
			if o.id==id then
				table.remove(self._listeners, i)
				break
			end
		end
	end
end

function Class:sendEvent(event)	
	for _, o in ipairs(self._listeners) do
		o.f(event)
	end
end



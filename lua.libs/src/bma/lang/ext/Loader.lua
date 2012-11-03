-- bma/lang/ext/Loader.lua

package.addLoader = function(fun)
	local f = function(modulename)
		local errmsg = ""
		local fcontent, fname = fun(modulename)
		if fcontent then
			if not fname then fname = modulename end
			-- Compile and return the module
		    return assert(loadstring(assert(fcontent), fname))	    
		end
		return nil		    
	end
	table.insert(package.loaders, 2, f)	
end


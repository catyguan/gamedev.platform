---- json
require("bma.lang.ext.Core")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Loader")

local f = function(modulename)
	if modulename=="test1" then
		return "print(1234)", "customLoader"
	end
end

package.addLoader(f)

require("test1")
require("test2")

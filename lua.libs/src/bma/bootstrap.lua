CONFIG = {}
CONFIG.DEBUG = false

local M = {}

M.run = function()
	-- avoid memory leak
	collectgarbage("setpause", 100)
	collectgarbage("setstepmul", 5000)

	-- init
	require("bma.lang.ext.Core")
	require("bma.lang.ext.Dump")
    
	local cfg = M.config or 'config'
	require(cfg)
	
	-- init Logger
	require("bma.lang.Logger")
	local Logger = bma.lang.Logger
    
    if CONFIG.Log and CONFIG.Log.type then
    	local log
    	local type = CONFIG.Log.type
    	
    	if type=="console" then
    		log = Logger.console(CONFIG.Log.logPattern)
    	end
    	if type=="print" then
    		log = Logger.luaprint(CONFIG.Log.logPattern)
    	end
    	if type=="file" then
    		log = Logger.file(
    			CONFIG.Log.filename,
    			CONFIG.Log.datePattern,
    			CONFIG.Log.logPattern
    			)
    	end
    	
    	if log then
    		_G.LOG = log
			_G.LOG:setLevel(CONFIG.Log.level or Logger.DEBUG)
    	end	    
    end
    
    if not _G.LOG then
		_G.LOG = Logger.console()
		_G.LOG:setLevel(Logger.DEBUG)
	end
	
	-- init services
	if CONFIG.Services then
		for _,sinfo in ipairs(CONFIG.Services) do
			local sname
			local sparam
			if type(sinfo)=="string" then
				sname = sinfo
			end
			if type(sinfo)=="table" then
				sname = sinfo.name
				sparam = sinfo				
			end
			LOG:info("BOOTSTRAP","install service "..sname)
			local s = class.forName(sname)
			s.install(sparam)
		end
	end
end

return M
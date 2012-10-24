-- bma/lang/Logger.lua

local _tostring = tostring
module("bma.lang.Logger", package.seeall)

-- const
DEBUG = "DEBUG"
INFO = "INFO"
WARN = "WARN"
ERROR = "ERROR"

local LEVEL = {"DEBUG", "INFO", "WARN", "ERROR"}
local MAX_LEVELS = #LEVEL
-- make level names to order
local i
for i=1,MAX_LEVELS do
	LEVEL[LEVEL[i]] = i
end

-- private log function, with support for formating a complex log message.
local function LOG_MSG(self, level, category, fmt, ...)
	local f_type = type(fmt)
	if f_type == 'string' then
		if select('#', ...) > 0 then
			local done,s = pcall(string.format,fmt, ...)
			if not done then s = "format error: "..fmt.."" end
			return self:append(level, category, s)
		else
			-- only a single string, no formating needed.
			return self:append(level, category, fmt)
		end
	elseif f_type == 'function' then
		-- fmt should be a callable function which returns the message to log
		return self:append(level, category, fmt(...))
	end
	-- fmt is not a string and not a function, just call tostring() on it.
	return self:append(level, category, tostring(fmt))
end

-- create the proxy functions for each log level.
local LEVEL_FUNCS = {}
for i=1,MAX_LEVELS do
	local level = LEVEL[i]
	LEVEL_FUNCS[i] = function(self, category, ...)
		-- no level checking needed here, this function will only be called if it's level is active.
		return LOG_MSG(self, level, category, ...)
	end
end

-- do nothing function for disabled levels.
local function disable_level() end
local function enabled_true() return true end
local function enabled_false() return false end

-- improved assertion funciton.
local function assert(exp, ...)
	-- if exp is true, we are finished so don't do any processing of the parameters
	if exp then return exp, ... end
	-- assertion failed, raise error
	error(format(...), 2)
end

-------------------------------------------------------------------------------
-- Creates a new logger object
-- @param append Function used by the logger to append a message with a
--	log-level to the log stream.
-- @return Table representing the new logger object.
-------------------------------------------------------------------------------
function new(append)

	if type(append) ~= "function" then
		return nil, "Appender must be a function."
	end

	local logger = {}
	logger.append = append

	logger.setLevel = function (self, level)
		local order = LEVEL[level]
		assert(order, "undefined level `%s'", _tostring(level))
		self.level = level
		self.level_order = order
		-- enable/disable levels
		for i=1,MAX_LEVELS do
			local name = LEVEL[i]:lower()
			self[LEVEL[i]] = LEVEL[i]
			if i >= order then
				self[name] = LEVEL_FUNCS[i]
				self[name.."Enabled"] = enabled_true
			else
				self[name] = disable_level
				self[name.."Enabled"] = enabled_false
			end
		end
	end

	-- generic log function.
	logger.log = function (self, level, category, ...)
		local order = LEVEL[level]
		assert(order, "undefined level `%s'", _tostring(level))
		if order < self.level_order then
			return
		end
		return LOG_MSG(self, level, category, ...)
	end

	-- initialize log level.
	logger:setLevel(DEBUG)
	return logger
end

function create(log,category)

	local r = {}

	r._log = log
	r._category = category
	for i=1,MAX_LEVELS do
		r[LEVEL[i]] = LEVEL[i]
		r[LEVEL[i]:lower()] = function(self, ...)
			return self:log(LEVEL[i], ...)
		end
	end

	r.log = function (self, level, ...)
		return self._log:log(level,self._category, ...)
	end
	return r
end


-------------------------------------------------------------------------------
-- Prepares the log message
-------------------------------------------------------------------------------
function prepareLogMsg(pattern, dt, level, category, message)

	local logMsg = pattern or "%date [%category] %level - %message\n"
	message = string.gsub(message, "%%", "%%%%")
	logMsg = string.gsub(logMsg, "%%date", dt)
	logMsg = string.gsub(logMsg, "%%category", category)
	logMsg = string.gsub(logMsg, "%%level", level)
	logMsg = string.gsub(logMsg, "%%message", message)
	return logMsg
end


-------------------------------------------------------------------------------
-- Converts a Lua value to a string
--
-- Converts Table fields in alphabetical order
-------------------------------------------------------------------------------
function tostring(value)
	local str = ''

	if (type(value) ~= 'table') then
		if (type(value) == 'string') then
			str = string.format("%q", value)
		else
			str = _tostring(value)
		end
	else
		local auxTable = {}
		table.foreach(value, function(i, v)
			if (tonumber(i) ~= i) then
				table.insert(auxTable, i)
			else
				table.insert(auxTable, tostring(i))
			end
		end)
		table.sort(auxTable)

		str = str..'{'
		local separator = ""
		local entry = ""
		table.foreachi (auxTable, function (i, fieldName)
			if ((tonumber(fieldName)) and (tonumber(fieldName) > 0)) then
				entry = tostring(value[tonumber(fieldName)])
			else
				entry = fieldName.." = "..tostring(value[fieldName])
			end
			str = str..separator..entry
			separator = ", "
		end)
		str = str..'}'
	end
	return str
end

function console(logPattern)
	return new(  function(self, level, category, message)
		io.stdout:write(prepareLogMsg(logPattern, os.date(), level, category, message))
		return true
	end
	)
end

function luaprint(logPattern)
	return new(  function(self, level, category, message)
		print(prepareLogMsg(logPattern, os.date(), level, category, message))
		return true
	end
	)
end

function file(filename, datePattern, logPattern)

	if type(filename) ~= "string" then
		filename = "lualogging.log"
	end

	return new( function(self, level,category, message)
		local f
		if self.file == nil then
			local filename = string.format(filename, os.date(datePattern))
			f = io.open(filename, "a")
			if (f) then
				f:setvbuf ("no")
				self.file = f
			else
				error(string.format("file `%s' could not be opened for writing", filename))
			end
		else
			f = self.file
		end
		local s = prepareLogMsg(logPattern, os.date(), level, category, message)
		f:write(s)
		f:write("\n")
		return true
	end
	)
end
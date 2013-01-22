-- bma/matrix/BoxMatrix.lua
-- 遵循[物理守则：通过相同的一个初始化数据和输入命令，能够准确还原各个时刻的情况]
require("bma.matrix.Matrix")

local LDEBUG = LOG:debugEnabled()
local LTAG = "BoxMatrix"

-- macro
RAND = function(m,n)
	return MT():random(m,n)
end

-- <<class BoxMatrix>>
local Class = class.define("bma.matrix.BoxMatrix",{bma.matrix.Matrix})

function Class:ctor()
	self:prop("commandId",1)
	self.commands = {}	
end

-- override
function Class:handleCommand(cmd)
	return false
end

-- override
function Class:handleHistory(data)
	if data.cmd then
		-- command
		self:postCommand(data.cmd)
		return true
	end
	if data.rseed then		
		LOG:debug(LTAG,"rseed - %d", data.rseed)
		math.randomseed(data.rseed)
		self:runv("rseed",true)
		return true
	end
	return false
end

function Class:historyWriter(writer)
	return self:prop("historyWriter", writer)
end
function Class:historyReader(reader)
	return self:prop("historyReader", reader)
end

function Class:processHistory(step)
	local reader = self:prop("historyReader")
	if not reader then return true end
	
	self:runv("historyMode", true)
	local r = false
	for i=1,step,1 do
		local data = reader()
		if not data then 
			r = true
			break
		end
		self:handleHistory(data)		
	end
	self:removeRunv("historyMode")
	return r
end

function Class:postCommand(pcmd)
	if self.cCMD then
		table.insert(self.commands,pcmd)
		return
	end
	
	self.cCMD = pcmd	
	while self.cCMD do
		cmd = self.cCMD
		if not cmd.id then
			cmd.id = self._prop.commandId
			self._prop.commandId = self._prop.commandId + 1		
		else
			self._prop.commandId = cmd.id
		end
		if self:handleCommand(cmd) then
			if not self:runv("historyMode") then		
				self:writeHistory({cmd=cmd})
			end
		end
		if #self.commands then
			self.cCMD = table.remove(self.commands,1)
		end
	end    
end

function Class:random(m,n)
	if not self:runv("rseed") then		
		local seed = os.mclock()
		LOG:debug(LTAG,"create rseed - %d", seed)
		self:writeHistory({rseed=seed})
		math.randomseed(seed)
		self:runv("rseed",true)
	end
	return math.random(m,n)
end

function Class:writeHistory(data)
	self._prop.historyWriter(data)
end

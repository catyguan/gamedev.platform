-- bma/matrix/BoxMatrix.lua
-- 遵循[物理守则：通过相同的一个初始化数据和输入命令，能够准确还原各个时刻的情况]
require("bma.matrix.Matrix")

local LDEBUG = LOG:debugEnabled()
local LTAG = "BoxMatrix"

-- <<class BoxMatrix>>
local Class = class.define("bma.matrix.BoxMatrix",{bma.matrix.Matrix})

function BoxMatrix:ctor()
	
end


-- ge/daemon/MazeGenerator.lua
-- 迷宫（地图）生成器

require("bma.lang.Class")
require("bma.lang.ext.Table")

-- <<class - MazeGenerator>>
local LDEBUG = LOG:debugEnabled()
local LTAG = "MazeGenerator"
local MazeGenerator = class.define("ge.daemon.generator.MazeGenerator")

function MazeGenerator:ctor()
    
end


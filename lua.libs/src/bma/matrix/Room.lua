-- bma/matrix/Room.lua
require("bma.lang.Class")
require("bma.lang.ext.Dump")
require("bma.lang.ext.Table")

local LDEBUG = LOG:debugEnabled()
local LTAG = "Space"

-- <<class RoomManager>>
local RoomManager = class.define("bma.matrix.RoomManager")



-- <<class Room>>
local Room = class.define("bma.matrix.Room", {bma.lang.StdObject})

function Room:ctor()
	
end


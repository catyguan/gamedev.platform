-- ge/arena/Effect.lua

require("bma.lang.Class")
require("ge.arena.VItem")

local Class = class.define("ge.arena.Effect",{ge.arena.CharComponent})

-- interface
function Class:buildCombat(ctype, ob, data, opponent) end
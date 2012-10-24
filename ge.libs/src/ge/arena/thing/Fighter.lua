-- ge/BaseLiving.lua

module(..., package.seeall)

require("bma.lang.Core")

BaseLiving = class()

function new()
    return BaseLiving.new()
end

function BaseLiving:ctor()    
    -- 唯一编号
    self.id = ""
    -- 标题名称
    self.name = ""
    -- 阵营，区分敌人和朋友
    self.faction = ""
end

function BaseLiving:dump()
    require("bma.lang.Dump")
    return string.dump(self)
end
-- ge/BaseLiving.lua

module(..., package.seeall)

require("bma.lang.Core")

BaseLiving = class()

function new()
    return BaseLiving.new()
end

function BaseLiving:ctor()    
    -- Ψһ���
    self.id = ""
    -- ��������
    self.name = ""
    -- ��Ӫ�����ֵ��˺�����
    self.faction = ""
end

function BaseLiving:dump()
    require("bma.lang.Dump")
    return string.dump(self)
end
-- ge/daemon/generator/ObjectGenerator.lua
-- 物体生成器

require("bma.lang.Class")
require("bma.lang.ext.Table")
require("bma.lang.ext.Dump")

local LDEBUG = LOG:debugEnabled()
local LTAG = "ObjectGenerator"

-- <<class - ObjectGenerator>>
local ObjectGenerator = class.define("ge.daemon.generator.ObjectGenerator",{})

function ObjectGenerator:ctor()
    
end

-- return weight(nil,*,1-10000),data
function ObjectGenerator:evaluate(ctx,cost,...)
    return self:evalRules(ctx,cost)
end

function ObjectGenerator:evalRules(ctx,cost,...)
    if self.rules~=nil then
        local slist = {}
        local clist = {}
        local total = 0
        for i,rule in ipairs(self.rules) do
            local w,d = rule:evaluate(ctx,cost,...)
            if w~=nil then
                if w=="*" then
                    table.insert(clist,{i=i,rule=rule,data=d})
                else
                    total = total + w
                    table.insert(slist,{i=i,weight=w,rule=rule,data=d})
                end
            end            
        end
        if #slist>0 then            
            local win = MT():random(1,total)
            -- print(LTAG,win,total)
            local r = nil
            for _,v in ipairs(slist) do
                if v.weight>=win then
                    r = v
                    break
                else
                    win = win - v.weight
                end
            end
            if LDEBUG then
                LOG:debug(LTAG,"'%s' evaluate - [%.2f,%s]",tostring(self.tag),r.weight/total*100, string.dump(r.rule))
            end
            return r.weight,r
        end
        if #clist>0 then 
            local win = MT():random(1,#clist)
            local r = clist[win]
            if LDEBUG then
                LOG:debug(LTAG,"'%s' evaluate - [*,%s]",tostring(self.tag),string.dump(r.rule))
            end
            return "*",clist[win]
        end
    end
    return nil
end

-- return remainCost,obj
function ObjectGenerator:generate(ctx,cost,data)
    if data==nil then
        local w,d = self:evalRules(ctx,cost)
        if d==nil then
            LOG:debug(LTAG,"'%s' - evaluate fail",tostring(self.tag))
            return cost,nil
        end
        data = d
    end
    local rule = self.rules[data.i]    
    return rule:generate(ctx, cost, data.data)
end

function ObjectGenerator:addRule(rule)
    if self.rules == nil then self.rules = {} end
    table.insert(self.rules, rule)
end

function ObjectGenerator:removeRule(f)
    table.walkRemove(self.rules, f)
end
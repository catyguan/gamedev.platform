-- bma/lang/Localization.lua

module(..., package.seeall)

function s(n,m)
	if m==nil then m = "string" end
	local modLang = require("res/string/"..m.."_"..(CONFIG.L.lang))
	local r = modLang[n]
	if r == nil then LOG:warn("Localization","LS("..n..","..m..") is nil") end
	return r
end

function labelBMFont(n,m)
	local fm = m
	if fm==nil then fm = "font" end
	local r = CCLabelBMFont:create(s(n,m),"res/string/"..fm.."_"..(CONFIG.L.lang)..".fnt");
	if r == nil then LOG:warn("Localization","labelBMFont("..n..","..tostring(m)..") is nil") end
	return r
end
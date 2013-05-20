-- bma/dotnet/app/Application.lua
require("bma.lang.ext.Core")
require("bma.lang.ext.String")
require("bma.app.ApplicationBase")

local Class = class.define("bma.dotnet.app.Application",{bma.app.ApplicationBase})
AppClass = Class

function Class:ctor()

end
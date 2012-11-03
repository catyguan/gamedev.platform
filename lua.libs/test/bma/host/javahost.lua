require("bma.test.bootstrap")

local javaHS
javaHS = class.forName("bma.host.java.Service")
javaHS.install()

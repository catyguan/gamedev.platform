local boot = require("bma.bootstrap")

boot.config = "bma.test.config4test"
boot.runTestcase()

LOG:debug("test","start debug")
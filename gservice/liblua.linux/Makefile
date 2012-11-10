# makefile for installing Lua
# see Makefile.a and ../lua/luaconf.h for further customization

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

LUA_DIR=../lua
LOCAL_DIR=../liblua.linux

# == END OF USER SETTINGS. NO NEED TO CHANGE ANYTHING BELOW THIS LINE =========

# Convenience platforms targets.
PLATS= aix ansi bsd freebsd generic linux macosx mingw posix solaris

# Lua version and release.
V= 5.1
R= 5.1.4

all:	$(PLAT)

$(PLATS):
	cp Makefile.a $(LUA_DIR)/Makefile
	cd $(LUA_DIR) && $(MAKE) $@
	mkdir -p $(LOCAL_DIR)/lib
	cp $(LUA_DIR)/liblua.a $(LOCAL_DIR)/lib/
	cd $(LUA_DIR) && $(MAKE) clean
	rm $(LUA_DIR)/Makefile

clean:
	cp Makefile.a $(LUA_DIR)/Makefile
	cd $(LUA_DIR) && $(MAKE) clean 
	rm $(LUA_DIR)/Makefile
	rm -rf $(LOCAL_DIR)/lib

none:
	@echo "Please do"
	@echo "   make PLATFORM"
	@echo "where PLATFORM is one of these:"
	@echo "   $(PLATS)"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) clean none

# (end of Makefile)


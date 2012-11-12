# makefile for building Lua
# see ../INSTALL for installation instructions
# see ../Makefile and luaconf.h for further customization

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

#JAVA_HOME=/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Home
JAVA_HOME=/usr/local/jdk1.6.0_31
LUA_DIR=../lua
LL_DIR=../lualibs
JNI_DIR=../jni

LUA_A_DIR=../liblua.linux/lib
LOCAL_DIR=../luajni.linux

SONAME=soname
#SONAME=install_name
SOEXT=so
#SOEXT=jnilib

# == END OF USER SETTINGS. NO NEED TO CHANGE ANYTHING BELOW THIS LINE =========

INC=-I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux" -I"$(LUA_DIR)"

PLATS= linux macosx

default: $(PLAT)

clean:
	rm -f *.o
	rm -f *.$(SOEXT) 

none:
	@echo "Please choose a platform:"
	@echo "   $(PLATS)"

linux macosx:
	gcc -c $(INC) $(LL_DIR)/md5/des56.c $(LL_DIR)/md5/ldes56.c $(LL_DIR)/md5/md5.c $(LL_DIR)/md5/md5lib.c
	gcc -c -O3 -Wall -pedantic -DNDEBUG -DUSE_INTERNAL_FPCONV $(INC) $(LL_DIR)/lua-cjson/g_fmt.c $(LL_DIR)/lua-cjson/lua_cjson.c $(LL_DIR)/lua-cjson/strbuf.c $(LL_DIR)/lua-cjson/dtoa.c
	gcc -c $(INC) $(JNI_DIR)/luajni.c
	g++ $(INC) -o libluajni.$(SOEXT) -shared -Wl,-$(SONAME),luajni.$(SOEXT) *.o $(LUA_A_DIR)/liblua.a -static -lc
	rm *.o

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: $(PLATS) default clean none buildit

# (end of Makefile)

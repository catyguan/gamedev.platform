LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dxlua_static

LOCAL_MODULE_FILENAME := libcocos2dxlua

LOCAL_SRC_FILES := \
$(LOCAL_PATH)/../lualibs/md5/des56.c \
$(LOCAL_PATH)/../lualibs/lua-cjson/dtoa.c \
$(LOCAL_PATH)/../lualibs/lua-cjson/g_fmt.c \
$(LOCAL_PATH)/../lua/lapi.c \
$(LOCAL_PATH)/../lua/lauxlib.c \
$(LOCAL_PATH)/../lua/lbaselib.c \
$(LOCAL_PATH)/../lua/lcode.c \
$(LOCAL_PATH)/../lua/ldblib.c \
$(LOCAL_PATH)/../lua/ldebug.c \
$(LOCAL_PATH)/../lualibs/md5/ldes56.c \
$(LOCAL_PATH)/../lua/ldo.c \
$(LOCAL_PATH)/../lua/ldump.c \
$(LOCAL_PATH)/../lua/lfunc.c \
$(LOCAL_PATH)/../lua/lgc.c \
$(LOCAL_PATH)/../lua/linit.c \
$(LOCAL_PATH)/../lua/liolib.c \
$(LOCAL_PATH)/../lua/llex.c \
$(LOCAL_PATH)/../lua/lmathlib.c \
$(LOCAL_PATH)/../lua/lmem.c \
$(LOCAL_PATH)/../lua/loadlib.c \
$(LOCAL_PATH)/../lua/lobject.c \
$(LOCAL_PATH)/../lua/lopcodes.c \
$(LOCAL_PATH)/../lua/loslib.c \
$(LOCAL_PATH)/../lua/lparser.c \
$(LOCAL_PATH)/../lua/lstate.c \
$(LOCAL_PATH)/../lua/lstring.c \
$(LOCAL_PATH)/../lua/lstrlib.c \
$(LOCAL_PATH)/../lua/ltable.c \
$(LOCAL_PATH)/../lua/ltablib.c \
$(LOCAL_PATH)/../lua/ltm.c \
$(LOCAL_PATH)/../lua/lua.c \
$(LOCAL_PATH)/../lualibs/lua-cjson/lua_cjson.c \
$(LOCAL_PATH)/../lualibs/luaext/luaext.c \
$(LOCAL_PATH)/../lua/lundump.c \
$(LOCAL_PATH)/../lua/lvm.c \
$(LOCAL_PATH)/../lua/lzio.c \
$(LOCAL_PATH)/../lualibs/md5/md5.c \
$(LOCAL_PATH)/../lualibs/md5/md5lib.c \
$(LOCAL_PATH)/../lua/print.c \
$(LOCAL_PATH)/../lualibs/lua-cjson/strbuf.c

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../lua

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../lua

include $(BUILD_STATIC_LIBRARY)

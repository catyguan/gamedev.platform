LOCAL_PATH := $(call my-dir)

C2DX_PATH := /cygdrive/c/workroom/cocos2d-x/cocos2d-x

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dxext_static

LOCAL_MODULE_FILENAME := libcocos2dext

LOCAL_SRC_FILES := \
Classes/CCEAction.cpp \
Classes/CCEApplication.cpp \
Classes/CCEApplication4CreateObject.cpp \
Classes/CCEAppUtil.cpp \
Classes/CCEButton.cpp \
Classes/CCEContainer.cpp \
Classes/CCEControl.cpp \
Classes/CCEDialog.cpp \
Classes/CCEDialogue.cpp \
Classes/CCEDirector.cpp \
Classes/CCEFSLuaLoader.cpp \
Classes/CCEHttpClient.cpp \
Classes/CCELabelButton.cpp \
Classes/CCELayerTouch.cpp \
Classes/CCELayoutUtil.cpp \
Classes/CCELuaApplication.cpp \
Classes/CCENarrate.cpp \
Classes/CCERectsNode.cpp \
Classes/CCEScene.cpp \
Classes/CCEScrollView.cpp \
Classes/CCESpriteFrameUtil.cpp \
Classes/CCETouch.cpp \
Classes/CCEUtil.cpp \
Classes/CCO4File.cpp \
Classes/CCO4HttpClient.cpp \
Classes/CCScale9Sprite.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Classes

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)../../../gservice/curl
LOCAL_C_INCLUDES += $(LOCAL_PATH)../../../gservice/lua
LOCAL_C_INCLUDES += $(C2DX_PATH)/cocos2dx
LOCAL_C_INCLUDES += $(C2DX_PATH)/cocos2dx/include
LOCAL_C_INCLUDES += $(C2DX_PATH)/cocos2dx/kazmath/include
LOCAL_C_INCLUDES += $(C2DX_PATH)/cocos2dx/platform/android
LOCAL_C_INCLUDES += $(C2DX_PATH)/extensions

include $(BUILD_STATIC_LIBRARY)

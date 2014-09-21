LOCAL_PATH := $(call my-dir)

#
# Gideros Shared Library
# 
include $(CLEAR_VARS)

LOCAL_MODULE            := gideros
LOCAL_SRC_FILES         := ../../libs/$(TARGET_ARCH_ABI)/libgideros.so

include $(PREBUILT_SHARED_LIBRARY)

#
# Lua Socket Library
# 
include $(CLEAR_VARS) 
LOCAL_MODULE            := luasocket
LOCAL_SRC_FILES         := ../../libs/$(TARGET_ARCH_ABI)/libluasocket.so 
include $(PREBUILT_SHARED_LIBRARY) 
#
# Lua File System Library
#
include $(CLEAR_VARS) 
LOCAL_MODULE            := lfs
LOCAL_SRC_FILES         := ../../libs/$(TARGET_ARCH_ABI)/liblfs.so 
include $(PREBUILT_SHARED_LIBRARY)
#
# Lua SQLite
#
include $(CLEAR_VARS) 
LOCAL_MODULE            := lsqlite3
LOCAL_SRC_FILES         := ../../libs/$(TARGET_ARCH_ABI)/liblsqlite3.so 
include $(PREBUILT_SHARED_LIBRARY)

#
# Plugin
#
include $(CLEAR_VARS)

LOCAL_MODULE           := ouya
LOCAL_ARM_MODE         := arm
LOCAL_CFLAGS           := -O2
LOCAL_SRC_FILES        := ouya.cpp ouyabinder.cpp
LOCAL_LDLIBS           := -ldl -llog
LOCAL_SHARED_LIBRARIES := gideros

include $(BUILD_SHARED_LIBRARY)

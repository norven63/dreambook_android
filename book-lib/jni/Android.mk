LOCAL_PATH := $(call my-dir)
TOP_LOCAL_PATH := $(LOCAL_PATH)


include $(CLEAR_VARS)
LOCAL_MODULE := Testa
LOCAL_SRC_FILES := libiconv.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Testb
LOCAL_SRC_FILES := libengine.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Testc
LOCAL_SRC_FILES := libextlib.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Testd
LOCAL_SRC_FILES := libmylib.a
include $(PREBUILT_STATIC_LIBRARY)

# Build shared library

include $(CLEAR_VARS)

LOCAL_MODULE    := testiconv

LOCAL_STATIC_LIBRARIES := Testb Testd Testc Testa

#LOCAL_SHARED_LIBRARIES := libiconv

MY_ROOT:=.
LOCAL_C_INCLUDES := \
	$(MY_ROOT)/extlib/tinyxml \
	$(MY_ROOT)/mylib \
	$(MY_ROOT)/engine \
	
LOCAL_SRC_FILES :=jni.cpp\


LOCAL_LDLIBS    := -lm -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)

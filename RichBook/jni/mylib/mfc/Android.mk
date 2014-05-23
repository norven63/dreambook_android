LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

STLPORT_BASE := /stlport
LOCAL_MODULE    := libmfc

LOCAL_CFLAGS += -I$(STLPORT_BASE)/stlport -D__NEW__ -D__SGI_STL_INTERNAL_PAIR_H  -DANDROID -DOS_ANDROID -DANDROID_NDK\
				-Wno-psabi -O -O2\

LOCAL_LDLIBS += -lz -llog


LOCAL_SRC_FILES := \
				./mArchive.cpp\
				./mBaseType.cpp\
				./mBmp.cpp\
				./mCore.cpp\
				./mFile.cpp\
				./mCollection.cpp\
				./mGlobal.cpp\
				./mICU.cpp\
				./mInet.cpp\
				./mIni.cpp\
				./mFunc1.cpp\
				./mFont.cpp\
				./mMsg.cpp\
				./mSync.cpp\
				./IniFile.cpp\
				./jmemsrc.cpp\
				./JPEGReader.cpp\
				./JPEGWriter.cpp\
				./PngImage.cpp\
				./UTF.cpp\
				./Socket.cpp\
				./mSearchUtil.cpp\
				./gif/dgif_lib.c\
				./gif/gif_err.c\
				./gif/gifalloc.c\
				./tga.cpp\
			
				

LOCAL_STATIC_LIBRARIES :=\
				libstlport\
				libpng\
				libjpeg\
				libexpat\
				
include $(BUILD_STATIC_LIBRARY)

# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_PLATFORM=android-8
APP_ABI := armeabi #armeabi-v7a
# APP_OPTIM := debug
APP_OPTIM :=release
APP_CFLAGS+= -DANDROID
APP_STL :=stlport_static   #gnustl_static
#STLPORT_FORCE_REBUILD := true
#APP_CPPFLAGS += -fexceptions




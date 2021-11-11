LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := tinyalsa 
LOCAL_SRC_FILES := libtinyalsa.a
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cc .cpp
LOCAL_C_INCLUDES :=$(LOCAL_PATH)
LOCAL_MODULE    := tinycap
LOCAL_STATIC_LIBRARIES += tinyalsa
LOCAL_SRC_FILES :=tinycap.cpp acosd.c colon.c FFTImplementationCallback.c main4.c main5_data.c main5_emxutil.c \
main5_initialize.c main5_terminate.c MBSS_computeAngularSpectrum.c MBSS_locate_spec.c MBSS_preprocess.c MBSS_qstft_multi.c \
minOrMax.c permute.c repmat.c rt_nonfinite.c rtGetInf.c rtGetNaN.c shiftdim.c sort.c sortIdx.c sph2cart.c sqrt.c sum.c
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_MODULE_TAGS := optional
#liblog.so libGLESv2.so
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)

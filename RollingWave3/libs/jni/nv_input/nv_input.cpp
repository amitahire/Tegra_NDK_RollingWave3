//----------------------------------------------------------------------------------
// File:        jni/nv_input/nv_input.cpp
// SDK Version: v10.10 
// Email:       tegradev@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2007-2012, NVIDIA CORPORATION.  All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------
#include "nv_input.h"
#include <android/log.h>
#include <dlfcn.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,  \
											 "NvInput", \
											 __VA_ARGS__))

typedef float(*NvGetAxesType)(const AInputEvent*, int32_t axis, size_t pointer_index);

static NvGetAxesType s_AMotionEvent_getAxisValue = NULL;
static bool s_AMotionEvent_getAxisValueInitialized = false;

bool NvInputInit()
{
	if (s_AMotionEvent_getAxisValueInitialized)
	{
		LOGD("Already initialized (%s)", 
			(s_AMotionEvent_getAxisValue != NULL) ? "Supported" : "Not supported");
		return s_AMotionEvent_getAxisValue != NULL;
	}

	s_AMotionEvent_getAxisValueInitialized = true;

    void* lib = dlopen("libandroid.so",0);
	if (!lib)
	{
		LOGD("Could not open libandroid.so");
		return false;
	}

    s_AMotionEvent_getAxisValue = (float(*)(const AInputEvent*,int32_t axis, size_t pointer_index))
        dlsym(lib,"AMotionEvent_getAxisValue");

	LOGD("Initialized (%s)", 
		(s_AMotionEvent_getAxisValue != NULL) ? "Supported" : "Not supported");

	return true;
}

bool NvInputGamepadSupported()
{
    return s_AMotionEvent_getAxisValue != NULL;
}

float NvInputGetAxisValue(const AInputEvent* motion_event,
        int32_t axis, size_t pointer_index)
{
    if (s_AMotionEvent_getAxisValue)
        return s_AMotionEvent_getAxisValue(motion_event, axis, pointer_index);
}


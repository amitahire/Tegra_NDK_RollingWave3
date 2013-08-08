//----------------------------------------------------------------------------------
// File:        jni/nv_input/nv_input.h
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
/* NVIDIA has modified this file to extend the behavior.  The original license
 * is provided below for reference and legal purposes.  
 */
#ifndef __NV_INPUT_H
#define __NV_INPUT_H

/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// IMPORTANT NOTES:
// - All Android games that include game controller support MUST include
//   some form of user axis/button mapping control panel.  This sample is
//   only designed to show the basics of game controller discovery, button
//   handling and a (non-NDK) method of accessing game controller axes.
//   Please contact NVIDIA developer relations for documentation and sample
//   code on Android game controller remapping.
//
// - The method of accessing analog game controller axes used in this demo
//   (the nv_input helper library) makes use of a dynamically-queried function
//   from libandroid.so that is not officially exported by the Android NDK
//   headers.  As such, it is not gauranteed to be supported in all versions of
//   Android or all Android platforms moving forward.  It has been tested with
//   success by NVIDIA on Android Honeycomb and Ice Cream Sandwich.  However,
//   in the name of full disclosure, it should be noted that this function is
//   not officially supported in the NDK.

#include <android/input.h>

/*
 * Constants that identify each individual axis of a motion event.
 * Refer to the documentation on the MotionEvent class for descriptions of each axis.
 */
enum {
    AMOTION_EVENT_AXIS_X = 0,
    AMOTION_EVENT_AXIS_Y = 1,
    AMOTION_EVENT_AXIS_PRESSURE = 2,
    AMOTION_EVENT_AXIS_SIZE = 3,
    AMOTION_EVENT_AXIS_TOUCH_MAJOR = 4,
    AMOTION_EVENT_AXIS_TOUCH_MINOR = 5,
    AMOTION_EVENT_AXIS_TOOL_MAJOR = 6,
    AMOTION_EVENT_AXIS_TOOL_MINOR = 7,
    AMOTION_EVENT_AXIS_ORIENTATION = 8,
    AMOTION_EVENT_AXIS_VSCROLL = 9,
    AMOTION_EVENT_AXIS_HSCROLL = 10,
    AMOTION_EVENT_AXIS_Z = 11,
    AMOTION_EVENT_AXIS_RX = 12,
    AMOTION_EVENT_AXIS_RY = 13,
    AMOTION_EVENT_AXIS_RZ = 14,
    AMOTION_EVENT_AXIS_HAT_X = 15,
    AMOTION_EVENT_AXIS_HAT_Y = 16,
    AMOTION_EVENT_AXIS_LTRIGGER = 17,
    AMOTION_EVENT_AXIS_RTRIGGER = 18,
    AMOTION_EVENT_AXIS_THROTTLE = 19,
    AMOTION_EVENT_AXIS_RUDDER = 20,
    AMOTION_EVENT_AXIS_WHEEL = 21,
    AMOTION_EVENT_AXIS_GAS = 22,
    AMOTION_EVENT_AXIS_BRAKE = 23,
    AMOTION_EVENT_AXIS_DISTANCE = 24,
    AMOTION_EVENT_AXIS_TILT = 25,
    AMOTION_EVENT_AXIS_GENERIC_1 = 32,
    AMOTION_EVENT_AXIS_GENERIC_2 = 33,
    AMOTION_EVENT_AXIS_GENERIC_3 = 34,
    AMOTION_EVENT_AXIS_GENERIC_4 = 35,
    AMOTION_EVENT_AXIS_GENERIC_5 = 36,
    AMOTION_EVENT_AXIS_GENERIC_6 = 37,
    AMOTION_EVENT_AXIS_GENERIC_7 = 38,
    AMOTION_EVENT_AXIS_GENERIC_8 = 39,
    AMOTION_EVENT_AXIS_GENERIC_9 = 40,
    AMOTION_EVENT_AXIS_GENERIC_10 = 41,
    AMOTION_EVENT_AXIS_GENERIC_11 = 42,
    AMOTION_EVENT_AXIS_GENERIC_12 = 43,
    AMOTION_EVENT_AXIS_GENERIC_13 = 44,
    AMOTION_EVENT_AXIS_GENERIC_14 = 45,
    AMOTION_EVENT_AXIS_GENERIC_15 = 46,
    AMOTION_EVENT_AXIS_GENERIC_16 = 47,

    // NOTE: If you add a new axis here you must also add it to several other files.
    //       Refer to frameworks/base/core/java/android/view/MotionEvent.java for the full list.
};

enum {
    AINPUT_SOURCE_CLASS_JOYSTICK = 0x00000010
};

enum {
    AINPUT_SOURCE_GAMEPAD = 0x00000400 | AINPUT_SOURCE_CLASS_BUTTON,
    AINPUT_SOURCE_JOYSTICK = 0x01000000 | AINPUT_SOURCE_CLASS_JOYSTICK,
};

bool NvInputInit();

bool NvInputGamepadSupported();

// See the note above; this function is implemented using a dynamically-queried
// function from the Android runtime that is NOT a part of the NDK.  It has been
// tested on current Android devices, but is not gauranteed on all platforms.
// Currently, pointer index should always be 0
float NvInputGetAxisValue(const AInputEvent* motion_event,
        int32_t axis, size_t pointer_index);


#endif // __NV_INPUT_H

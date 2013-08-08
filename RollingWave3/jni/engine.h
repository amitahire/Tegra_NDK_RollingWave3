//----------------------------------------------------------------------------------
// File:        native_basic/jni/engine.h
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
#ifndef __ENGINE_H
#define __ENGINE_H

#pragma once

#include "Common.h"
#include <nv_and_util/nv_native_app_glue.h>
#include <nv_egl_util/nv_egl_util.h>
#include <android/sensor.h>

//***************************Includes.***************************//
#include "triangle.h"

using namespace std;

#define APP_NAME "RollingWave/engine.h"

extern	float	msec;

class triangle;

class Engine
{
public:
	
	
	Engine(NvEGLUtil& egl, struct android_app* app);
	~Engine();

	bool isGameplayMode() { return mGameplayMode; }

	void updateFrame(bool interactible, long deltaTime);

	void looperIteration(int looperIdent);

protected:

	void setGameplayMode(bool paused);

	void advanceTime(long time) { mTimeVal += time * 0.001; }

	bool isForcedRenderPending() { return mForceRender > 0; }

	void requestForceRender() { mForceRender = 4; }

	bool checkWindowResized();
	
	//This msec is one passed to update the camera and stuff. Using the same deltaTime.
	bool renderFrame(bool allocateIfNeeded, float msec);

	static void handleCmdThunk(struct android_app* app, int32_t cmd);
	static int32_t handleInputThunk(struct android_app* app, AInputEvent* event);

	bool initUI();

	bool resizeIfNeeded();

	int handleInput(AInputEvent* event);

	void handleCommand(int cmd);

    struct android_app* mApp;

	NvEGLUtil& mEgl;

	////***************************Adding triangle.***************************//
	triangle* triangleApp;

	//test text.
	void* testText;
	void* textUPLeft;
	void* textUPRight;
	void* textDOWNLeft;
	void* textDOWNRight;
	void* textRIGHT;
	void* textLEFT;
	void* score;

	static void canonicalToWorld(int displayRotation, const ASensorVector& cannonicalVec, ASensorVector& worldVec);

	bool mResizePending;

	bool mGameplayMode;

	int mForceRender;
	
	double mTimeVal;

	bool m_uiInitialized;

	void *m_clockText;
	void *m_uiText[2];

	float camMoveX,camMoveY;
	float camPitch,camYaw;

	bool toggleCamera;

	ASensorManager* m_sensorManager;
    const ASensor* m_accelerometerSensor;
    ASensorEventQueue* m_sensorEventQueue;

    ASensorVector m_rawAccelerometer;
    ASensorVector m_worldAccelerometer;

	int mOrientation;

};


#endif // __ENGINE_H

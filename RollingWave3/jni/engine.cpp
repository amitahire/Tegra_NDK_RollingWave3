//----------------------------------------------------------------------------------
// File:        native_basic/jni/engine.cpp
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
#include "engine.h"

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>

#include <nv_and_util/nv_native_app_glue.h>
#include <nv_egl_util/nv_egl_util.h>
#include <nv_bitfont/nv_bitfont.h>
#include <nv_shader/nv_shader.h>

float msec = 0;



Engine::Engine(NvEGLUtil& egl, struct android_app* app) :
	mEgl(egl)
{
    mApp = app;

	//***************************Creating a object for triangle class. (triangle.cpp)***************************//
	camMoveX = 0.0f;
	camMoveY = 0.0f;
	camPitch = 0.0f;
	camYaw = 0.0f;

	triangleApp = NULL;
	triangleApp = new triangle();

	mResizePending = false;

	mGameplayMode = true;
	//mGameplayMode = false;

	mForceRender = 4;

    mTimeVal = 0.0;

	if(!mEgl.isBound())
	{
		LOGD("EGL context not bound.");
	}

	//***************************You have to fill this struct (app->) with info**************************************//
	//********************OnAppCmd - Basically controls all the android stages. OnCreate() etc - Check the struct.***//
	//********************OnInputEvent - Handle input stuff.*********************************************************//
	//***************************************************************************************************************//
    app->userData = this;
	app->onAppCmd = &Engine::handleCmdThunk; 
    app->onInputEvent = &Engine::handleInputThunk;

	m_uiInitialized = false;

	nv_shader_init(app->activity->assetManager);

	// Sensor stuff - Accelerometer.
	m_sensorManager = ASensorManager_getInstance();
    m_accelerometerSensor = ASensorManager_getDefaultSensor(m_sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    m_sensorEventQueue = ASensorManager_createEventQueue(m_sensorManager,
            mApp->looper, LOOPER_ID_USER, NULL, NULL);

    m_rawAccelerometer.x = 0.0f;
    m_rawAccelerometer.y = -9.8f;
    m_rawAccelerometer.z = 0.0f;

    m_worldAccelerometer.x = 0.0f;
    m_worldAccelerometer.y = -9.8f;
    m_worldAccelerometer.z = 0.0f;

	mOrientation = nv_app_get_display_rotation(mApp);


}

Engine::~Engine()
{
	NVBFTextFree(testText);
	NVBFCleanup();

}

//***************************Intializes the User Interface(UI). With fonts and stuff. Currently removed for being drawn.***************************//
bool Engine::initUI() 
{

	//setGameplayMode(true);

	//For HUD.
	#define NUM_FONTS	2
	static NvBool fontsSplit[NUM_FONTS] = {1,1}; /* all are split */
	static const char *fontFiles[NUM_FONTS] = {
	    "courier+lucida_256.dds",
	    "utahcond+bold_1024.dds"
	};
	if (NVBFInitialize(NUM_FONTS, (const char**)fontFiles, fontsSplit, 0))
	{
		LOGW("Could not initialize NvBitFont");
		return false;
	}

	testText = NVBFTextAlloc();
    NVBFTextSetFont(testText, 2); 
    NVBFTextSetSize(testText, 64);
	NVBFTextSetColor(testText, NV_PC_PREDEF_WHITE); //colour.

	textUPLeft = NVBFTextAlloc();
	NVBFTextSetFont(textUPLeft, 2);
    NVBFTextSetSize(textUPLeft, 64);
	NVBFTextSetColor(textUPLeft, NV_PC_PREDEF_WHITE);


	textUPRight = NVBFTextAlloc();
	NVBFTextSetFont(textUPRight, 2); 
    NVBFTextSetSize(textUPRight, 64);
	NVBFTextSetColor(textUPRight, NV_PC_PREDEF_WHITE); 

	textDOWNLeft = NVBFTextAlloc();
	NVBFTextSetFont(textDOWNLeft, 2); 
    NVBFTextSetSize(textDOWNLeft, 64);
	NVBFTextSetColor(textDOWNLeft, NV_PC_PREDEF_WHITE); 

	textDOWNRight = NVBFTextAlloc();
	NVBFTextSetFont(textDOWNRight, 2); 
    NVBFTextSetSize(textDOWNRight, 64);
	NVBFTextSetColor(textDOWNRight, NV_PC_PREDEF_WHITE); 

	textRIGHT = NVBFTextAlloc();
	NVBFTextSetFont(textRIGHT, 2); 
    NVBFTextSetSize(textRIGHT, 64);
	NVBFTextSetColor(textRIGHT, NV_PC_PREDEF_WHITE); 

	textLEFT = NVBFTextAlloc();
	NVBFTextSetFont(textLEFT, 2);
	NVBFTextSetSize(textLEFT, 64);
	NVBFTextSetColor(textLEFT, NV_PC_PREDEF_WHITE); 

	//score = NVBFTextAlloc();
	//NVBFTextSetFont(score, 2);
	//NVBFTextSetSize(score, 64);
	//NVBFTextSetColor(score, NV_PC_PREDEF_WHITE);


	return true;
}

void Engine::setGameplayMode(bool running)
{
	if (mGameplayMode != running)
		requestForceRender();

	//Check the values from accelerometer.
	if (m_accelerometerSensor != NULL)
		{

			if (running)        
			{
                ASensorEventQueue_enableSensor(m_sensorEventQueue,
                        m_accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(m_sensorEventQueue,
                        m_accelerometerSensor, (1000L/60)*1000);
            }
			else
			{
                ASensorEventQueue_disableSensor(m_sensorEventQueue,
                        m_accelerometerSensor);
            }
		}

	mGameplayMode = running;
}

bool Engine::checkWindowResized()
{
	if (mEgl.checkWindowResized())
	{
		mResizePending = true;
		requestForceRender();
		LOGI("Window size change %dx%d", mEgl.getWidth(), mEgl.getHeight());
		triangleApp->setWinHeight(mEgl.getHeight());
		triangleApp->setWinWidth(mEgl.getWidth());
		//LOGD("%d : %d ", mEgl.getHeight(), mEgl.getWidth());
		return true;
	}

	return false;
}

bool Engine::resizeIfNeeded()
{
	if (!mResizePending)
		return false;

	mResizePending = false;

	return true;
}

bool Engine::renderFrame(bool allocateIfNeeded, float deltaTime)
{
    if (!mEgl.isReadyToRender(allocateIfNeeded))
        return false;

	if (!initUI())
	{
		LOGW("Could not initialize UI - assets may be missing!");
		ANativeActivity_finish(mApp->activity);
		return false;
	}

	//*************************** Let set the basic triangle class and call its object.***************************//
	if (!triangleApp)
	{
		if(!allocateIfNeeded)
			return false;

		triangleApp = new triangle();
		
	}

	if(!triangleApp->isInitialized())
	{
		if(!allocateIfNeeded)
			return false;

		// If we are initializing and we have a new, known size, set that 
		// before the init.
		resizeIfNeeded();

		if(!triangleApp->init())
			return false;

		//return 0;
	}


	resizeIfNeeded();

	 
	//******************set up viewport*******************************//
	glViewport((GLint)0, (GLint)0, 
		(GLsizei)(mEgl.getWidth()), (GLsizei)(mEgl.getHeight()));

	// clear buffers as necessary
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	//******************Do some game rendering here*******************************//
	//***************** All the game updates done here.***************************//


	triangleApp->UpdateCore(deltaTime);
	triangleApp->UpdateGame(deltaTime);
	msec = deltaTime;

	//LOGD("%f", deltaTime); // About 30.

	//***********mForceRender - Loops through 4 times and then forces a render.*****//
	if (mForceRender > 0)
		mForceRender--;
	
	//////////////////////////////////////////////TEXT -- to display HUD.//////////////////////////////////////////////////////

	int w = mEgl.getWidth();
	int h = mEgl.getHeight();
	int height = (w > h) ? (h / 16) : (w / 16);

	NVBFSetScreenRes(w, h);
	char fpschar[32];
	char ch[10];
	
	if(testText)
	{
		NVBFTextSetSize(testText, height);
		NVBFTextCursorAlign(testText, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(testText, 1000, 150);
		float tempFPS = triangleApp->GetFPS();
		sprintf(fpschar, "FPS : %f", tempFPS);
		NVBFTextSetString(testText, fpschar);
	}

	//Left.
	if(textUPLeft)
	{
		NVBFTextSetSize(textUPLeft, height);
		NVBFTextCursorAlign(textUPLeft, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(textUPLeft, 50, 250);
		NVBFTextSetString(textUPLeft, "UP");
	}

	if(textDOWNLeft)
	{
		NVBFTextSetSize(textDOWNLeft, height);
		NVBFTextCursorAlign(textDOWNLeft, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(textDOWNLeft, 50, 550);
		NVBFTextSetString(textDOWNLeft, "DOWN");
	}

	if(textLEFT)
	{
		NVBFTextSetSize(textLEFT, height);
		NVBFTextCursorAlign(textLEFT, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(textLEFT, 50, 400);
		NVBFTextSetString(textLEFT, "LEFT");
	}

	//Right.

	if(textUPRight)
	{
		NVBFTextSetSize(textUPRight, height);
		NVBFTextCursorAlign(textUPRight, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(textUPRight, 1150, 250);
		NVBFTextSetString(textUPRight, "UP");
	}



	if(textDOWNRight)
	{
		NVBFTextSetSize(textDOWNRight, height);
		NVBFTextCursorAlign(textDOWNRight, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(textDOWNRight, 1150, 550);
		NVBFTextSetString(textDOWNRight, "DOWN");
	}

	if(textRIGHT)
	{
		NVBFTextSetSize(textRIGHT, height);
		NVBFTextCursorAlign(textRIGHT, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
		NVBFTextCursorPos(textRIGHT, 1150, 400);
		NVBFTextSetString(textRIGHT, "RIGHT");
	}

	//if(score)
	//{

	//	NVBFTextSetSize(score, height);
	//	NVBFTextCursorAlign(score, NVBF_ALIGN_LEFT, NVBF_ALIGN_BOTTOM);
	//	NVBFTextCursorPos(score, 50, 100);
	//	float fscore = triangleApp->GetPhysicsSystem().GetStuckballs();
	//	sprintf(ch, "STUCK BALLS : %f", fscore);
	//	NVBFTextSetString(score, ch);

	//}



	NVBFTextRenderPrep();
	NVBFTextRender(testText);
	NVBFTextRender(textUPLeft);
	NVBFTextRender(textUPRight);
	NVBFTextRender(textDOWNLeft);
	NVBFTextRender(textDOWNRight);
	NVBFTextRender(textRIGHT);
	NVBFTextRender(textLEFT);
	//NVBFTextRender(score);
	NVBFTextRenderDone();


	//LOGD("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX %f", m_worldAccelerometer.x);
	//LOGD("renderer %f", deltaTime);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    mEgl.swap();


    return true;
}

void Engine::updateFrame(bool interactible, long deltaTime)
{
	
	if (interactible)
	{
		// Each frame, we check to see if the window has resized.  While the
		// various events we get _should_ cover this, in practice, it appears
		// that the safest move across all platforms and OSes is to check at 
		// the top of each frame
		checkWindowResized();

		// Time stands still when we're auto-paused, and we don't
		// automatically render
		if (mGameplayMode)
		{
			//LOGD("TEST 1, %f", deltaTime);
			advanceTime(deltaTime);

			// This will try to set up EGL if it isn't set up
			// When we first set up EGL completely, we also load our GLES resources
			// If these are already set up or we succeed at setting them all up now, then
			// we go ahead and render.
			if(!renderFrame(true,deltaTime)){ return; }
			//LOGD("test"); - Being drawn alright.
			//LOGD("TEST 2, %f", deltaTime);
		}
		else if (isForcedRenderPending()) // forced rendering when needed for UI, etc
		{

			if(!renderFrame(true,deltaTime)){ return; }
		}
	}
	else
	{
		// Even if we are not interactible, we may be visible, so we
		// HAVE to do any forced renderings if we can.  We must also
		// check for resize, since that may have been the point of the
		// forced render request in the first place!
		if (isForcedRenderPending() && mEgl.isReadyToRender(false)) 
		{
			checkWindowResized();
			// Removing now - Having a fixed cam for a while.
			/*triangleApp->camera->UpdateCamera(deltaTime);
			triangleApp->viewMatrix = triangleApp->camera->BuildViewMatrix();*/

			if(renderFrame(true,deltaTime)){ return; }
		}
	}
}

int Engine::handleInput(AInputEvent* event)
{

	int32_t eventType = AInputEvent_getType(event);

	if (eventType == AINPUT_EVENT_TYPE_MOTION)
	{	
		int32_t action = AMOTION_EVENT_ACTION_MASK & AMotionEvent_getAction((const AInputEvent*)event);
		int32_t actionUnmasked = AMotionEvent_getAction(event);

		float pX = AMotionEvent_getX(event, 0);
		float pY = AMotionEvent_getY(event, 0);
		/*Debug */
		//LOGD("X COORD : %f", pX);
		//LOGD("Y COORD : %f", pY);
		
		if((pX > (mEgl.getWidth()/2 + 100)) && (pX < (mEgl.getWidth()/2 - 100)) && (pY < 100)){

			triangleApp->toggleCamera = !(triangleApp->toggleCamera);
			LOGD("TOUCH");
			//triangleApp->GetPhysicsSystem().GetCamera()->SetPosition(Vector3(2065, 2170, 1897));
			//triangleApp->GetPhysicsSystem().GetCamera()->SetYaw(1.05);
			//triangleApp->GetPhysicsSystem().GetCamera()->SetPitch(-26);


		}
		
		/*Split To Left and Right Screen*/
		/*First Block - Move camera up and down. */
		if (pY < 200) {
			if (pX > (mEgl.getWidth()/2)) {
				triangleApp->camera->SetPosition(triangleApp->camera->GetPosition() + Vector3 (0,10,0));
				triangleApp->viewMatrix = triangleApp->camera->BuildViewMatrix();		
				return 0;
			} else {
				triangleApp->camera->SetPosition(triangleApp->camera->GetPosition() - Vector3 (0,10,0));
				triangleApp->viewMatrix = triangleApp->camera->BuildViewMatrix();		
				return 0;
			}
		/*Panning 360 - Right Side of the screen.*/
		} else if (pX > (mEgl.getWidth()/2)) {
			if (action == AMOTION_EVENT_ACTION_DOWN) {
				LOGD("Engine : View Started");
				camYaw = pX;
				camPitch = pY;				
				return 0; 
			} else {
				float movedX = pX - camYaw;
				float movedY = pY - camPitch;
				movedX = movedX / 50;
				movedY = movedY / 50;

				/*Ball Movement*/
				/*Right*/
				//LOGD("X :%f, Y : %f", mEgl.getWidth(), mEgl.getHeight());;
				if( (pX >= 1080 && pX <= 1280) && (pY >= 300 && pY <= 500) ) {

					LOGD("--RIGHT--");
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetAtRestBool(false);	
					//PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetLinearVelocity(Vector3(100, 0 ,0));
					////Force.
					//PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->GetForce() 
					//	+ Vector3::Cross(Vector3(PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().x, 0.0f, PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().z) * 85.0f * msec, Vector3(0.0f, 1.0f, 0.0f)));

					////Torque.
					//PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque( PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->GetTorque()
					//	+ Vector3(PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().x * 10.8f * msec, 0.0f, PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().z * 10.8f * msec));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(Vector3(60, 0 ,0));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque(Vector3(30, 30, 30));

					

					////Force based on Camera.
					//PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->GetForce() 
					//	+ Vector3::Cross(Vector3(PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().x, 0.0f, PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().z) * 105.0f * msec, Vector3(0.0f, 1.0f, 0.0f)));

					////Torque based on Camera.
					//PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque( PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->GetTorque()
					//	+ Vector3(PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().x * 10.8f * msec, 0.0f, PhysicsSystem::GetPhysicsSystem().GetCamera()->Forward().z * 10.8f * msec));
					
				}

				/*Up on Right sides.*/
				if( ( (pX >= 0 && pX <= 200) || (pX >= 1080 && pX <= 1280)) && (pY >= 100 && pY <= 300)){

					LOGD("--UP--");
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(Vector3(0, 0 ,-60));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque(Vector3(30, 30, 30));
				}

				/* Down on Right side*/
				if( ( (pX >= 0 && pX <= 200) || (pX >= 1080 && pX <= 1280)) && (pY >= 500 && pY <= 700)){

					LOGD("--DOWN--");
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(Vector3(0, 0 ,60));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque(Vector3(30, 30, 30));
				}

				triangleApp->camera->SetPitch(triangleApp->camera->GetPitch() - movedY);
				triangleApp->camera->SetYaw(triangleApp->camera->GetYaw() - movedX);
				triangleApp->viewMatrix = triangleApp->camera->BuildViewMatrix();		
				return 1;		
			}
		/* Movement of the Camera - To the left of screen.*/
		} else {
			if (action == AMOTION_EVENT_ACTION_DOWN) {
				LOGD("Engine : Movement Started");
				camMoveX = pX;
				camMoveY = pY;
				return 0; 
			} else {
				float movedX = pX - camMoveX;
				float movedY = pY - camMoveY;
				float buttonSize = mEgl.getWidth() / 6.4; //200 for buttons
				//LOGD("Width(UP FRONT): %f", mEgl.getWidth());
				//LOGD("Height(UP FRONT): %f", mEgl.getHeight());
				
				/*triangleApp->camera->SetPosition(triangleApp->camera->GetPosition()+Vector3(movedX,0,movedY));*/

				/*Ball Movements.*/
				/*	For Asus Transform.
					X = Yaw = Left to Right = 0 - 1280
					Y = Pitch = Up to Down = 0 - 800*/

				/*Left*/
				if( (pX >= 0 && pX <= 200) && (pY >= 300 && pY <= 500) ) {

					LOGD("--LEFT--");
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(Vector3(-60, 0 ,0));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque(Vector3(-30, 0, 0));
				}


				/*Up on Left side.*/
				if( ( (pX >= 0 && pX <= 200) || (pX >= 1080 && pX <= 1280)) && (pY >= 100 && pY <= 300)){

					LOGD("--UP--");
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(Vector3(0, 0 ,-60));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque(Vector3(30, 30, 30));
				}

				/* Down on Left side.*/
				if( ( (pX >= 0 && pX <= 200) || (pX >= 1080 && pX <= 1280)) && (pY >= 500 && pY <= 700)){

					LOGD("--DOWN--");
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetForce(Vector3(0, 0 ,60));
					PhysicsSystem::GetPhysicsSystem().GetPointerToMainBall()->SetTorque(Vector3(30, 30, 30));
				}
				//LOGD("Button Size: %f", buttonSize);
				//LOGD("Width(Surface): %f", mEgl.getSurfaceWidth());
				LOGD("Format(Surface): %f", mEgl.getFormat());
				/*Up*/
				//if ( pY 
				if(movedY > -50) {
					triangleApp->camera->SetPosition(triangleApp->camera->GetPosition() - 
						Matrix4 :: Rotation(triangleApp->camera->GetYaw(),Vector3 (0.0f ,1.0f ,0.0f )) *
						Vector3 (0.0f ,0.0f , -20.0f ));
				}

				if(movedY < 50) {
					triangleApp->camera->SetPosition(triangleApp->camera->GetPosition() + 
						Matrix4 :: Rotation(triangleApp->camera->GetYaw(),Vector3 (0.0f ,1.0f ,0.0f )) *
						Vector3 (0.0f ,0.0f , -20.0f ));
				}

				if(movedX < -50) {
					triangleApp->camera->SetPosition(triangleApp->camera->GetPosition() + 
						Matrix4 :: Rotation(triangleApp->camera->GetYaw(),Vector3 (0.0f ,1.0f ,0.0f )) *
						Vector3 (-20.0f ,0.0f , 0.0f ));	
				}

				if(movedX > 50) {
					triangleApp->camera->SetPosition(triangleApp->camera->GetPosition() - 
						Matrix4 :: Rotation(triangleApp->camera->GetYaw(),Vector3 (0.0f ,1.0f ,0.0f )) *
						Vector3 (-20.0f ,0.0f , 0.0f ));	
				}

				triangleApp->viewMatrix = triangleApp->camera->BuildViewMatrix();		
				return 1;		
			}
		}
		

	} else if (eventType == AINPUT_EVENT_TYPE_KEY) {
		int32_t code = AKeyEvent_getKeyCode((const AInputEvent*)event);

		//TODO -Amit - Add a pause menu here. Check control.
		// if we are in gameplay mode, we eat the back button and move into
		// pause mode.  If we are already in pause mode, we allow the back
		// button to be handled by the OS, which means we'll be shut down
		if ((code == AKEYCODE_BACK) && mGameplayMode)
		{
			setGameplayMode(false);
			return 1;
		}
	}

    return 0;
}

void Engine::handleCommand(int cmd)
{
    switch (cmd)
    {
		// The window is being shown, get it ready.
		// Note that on ICS, the EGL size will often be correct for the new size here
		// But on HC it will not be.  We need to defer checking the new res until the
		// first render with the new surface!
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_WINDOW_RESIZED:
			mEgl.setWindow(mApp->window);
			requestForceRender();
        	break;

        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
			mEgl.setWindow(NULL);
			break;

        case APP_CMD_GAINED_FOCUS:
			requestForceRender();
			break;

        case APP_CMD_LOST_FOCUS:
		case APP_CMD_PAUSE:
        	// Move out of gameplay mode if we are in it.  But if we are
			// in another dialog mode, leave it as-is
            if (mGameplayMode)
				setGameplayMode(false);
			requestForceRender();
            break;

		// ICS does not appear to require this, but on GB phones,
		// not having this causes rotation changes to be delayed or
		// ignored when we're in a non-rendering mode like autopause.
		// The forced renders appear to allow GB to process the rotation
		case APP_CMD_CONFIG_CHANGED:
			requestForceRender();
			break;
    }
}

/**
 * Process the next input event.
 */
int32_t Engine::handleInputThunk(struct android_app* app, AInputEvent* event)
{
    Engine* engine = (Engine*)app->userData;
	if (!engine)
		return 0;

	return engine->handleInput(event);
}

/**
 * Process the next main command.
 */
void Engine::handleCmdThunk(struct android_app* app, int32_t cmd)
{
    Engine* engine = (Engine*)app->userData;
	if (!engine)
		return;

	engine->handleCommand(cmd);
}

/**
 * Accerelometer
 **/

void Engine::looperIteration(int looperIdent)
{
    if (looperIdent+2 == LOOPER_ID_USER) {
        if (m_accelerometerSensor != NULL) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(m_sensorEventQueue,
                    &event, 8) > 0) {
                m_rawAccelerometer = event.acceleration;
				// Transform into "rendering space" from the device-
				// relative cannonical space and cache in the engine object
                canonicalToWorld(mOrientation, m_rawAccelerometer, 
                    m_worldAccelerometer);
//#if 0
                LOGI("accelerometer: raw(%f, %f, %f), world(%f, %f, %f)",
                        m_rawAccelerometer.x, m_rawAccelerometer.y,
                        m_rawAccelerometer.z,
                        m_worldAccelerometer.x, m_worldAccelerometer.y,
                        m_worldAccelerometer.z);
//#endif	
			}
       }
    }
}

// Transforms the device-cannonical-space accelerometer vector to 
// current-screen-orientation-relative accelerometer.
void Engine::canonicalToWorld(int displayRotation, const ASensorVector& cannonicalVec, 
    ASensorVector& worldVec)
{
	// define a table to do the axis negate/swaps, 
	struct AxisSwap
	{
		signed char negateX;
		signed char negateY;
		signed char xSrc;
		signed char ySrc;
	};
	static const AxisSwap axisSwap[] = {
		{ 1,  1, 0, 1 },
		{-1,  1, 1, 0 },
		{-1, -1, 0, 1 },
		{ 1, -1, 1, 0 } };

	const AxisSwap& as = axisSwap[displayRotation];
	worldVec.v[0] = (float)as.negateX * cannonicalVec.v[ as.xSrc ];
	worldVec.v[1] = (float)as.negateY * cannonicalVec.v[ as.ySrc ];
	worldVec.v[2] = cannonicalVec.v[2];
}

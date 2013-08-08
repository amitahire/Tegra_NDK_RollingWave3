#include "MainBall.h"
#include "../Team Project/Team Project/Physics/PhysicsSystem.h"

MainBall::MainBall(void)
{
	sceneNode	= NULL;
	physicsNode = NULL;
	
}

MainBall::MainBall(SceneNode* s, PhysicsNode* p) {
	sceneNode	= s;
	physicsNode = p;
}


MainBall::~MainBall()
{
	
}


void MainBall::Update(float msec){

	psys = &PhysicsSystem::GetPhysicsSystem();
}

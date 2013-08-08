#include "CubeRobot.h"

Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void) {



	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 1));		//Red
	body->SetModelScale(Vector3(10, 10, 10));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, -100)));
 	AddChild(body);

	head	= new SceneNode(cube, Vector4(0, 1, 0, 1));				//Green 
	head->SetModelScale(Vector3(10, 10, 10));
	head->SetTransform(Matrix4::Translation(Vector3(10, 1, 1)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1));				//Blue
	leftArm->SetModelScale(Vector3(50, 50, 50));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-10, 1, 1)));
	body->AddChild(leftArm);

	

}

void CubeRobot::Update(float msec) {

	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(0, 1, 0)));
	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));
	//rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));


	SceneNode::Update(msec);

}


#include "CubeRobot.h"

Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void) {

	//Optional - Uncomment if you want a local origin marker - What does it mean? :P
	//SetMesh(cube);
	
	// So you need not to present the return type -- Check all other and leftarm. Both work.!!

	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 1));		//Red
	body->SetModelScale(Vector3(10, 10, 10));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, -100)));
 	AddChild(body);

	head	= new SceneNode(cube, Vector4(0, 1, 0, 1));				//Green -- Why??? With SceneNode* it gave error. Hmm I wonder why.
	head->SetModelScale(Vector3(10, 10, 10));
	head->SetTransform(Matrix4::Translation(Vector3(10, 1, 1)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1));				//Blue
	leftArm->SetModelScale(Vector3(50, 50, 50));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-10, 1, 1)));
	body->AddChild(leftArm);

	//rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1));				//Blue
	//rightArm->SetModelScale(Vector3(3, -18, 3));
	//rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	//body->AddChild(rightArm);

	//SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1));		//Blue
	//leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	//leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	//body->AddChild(leftLeg);

	//SceneNode* rightLeg	= new SceneNode(cube, Vector4(0, 0, 1, 1));
	//rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	//rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	//body->AddChild(rightLeg);


	////// Let me add the code for the other objects.
	////SceneNode* ico1 = new SceneNode(ico, Vector4(1, 0, 0, 1));
	////ico1->SetModelScale(Vector3(3, 10, 3));
	////ico1->SetTransform(Matrix4::Translation(Vector3(15, 0, 0)));
	////body->AddChild(ico1);

	//SceneNode* pyra1 = new SceneNode(pyra, Vector4(1, 1, 0, 1));
	//pyra1->SetModelScale(Vector3(150, 150, 150));
	//pyra1->SetTransform(Matrix4::Translation(Vector3(30, 30, 30)) * Matrix4::Rotation( 270.0f, Vector3(1, 0, 0)));
	////body->AddChild(pyra1);
	//AddChild(pyra1);

	///*SceneNode* ico1 = new SceneNode(ico, Vector4(1, 0, 0, 1));
	//ico1->SetModelScale(Vector3(3, 10, 3));
	//ico1->SetTransform(Matrix4::Translation(Vector3(15, 0, 0)));
	//body->AddChild(ico1);*/

	////SceneNode* pyra2 = new SceneNode(pyra, Vector4(1, 0, 0, 1));
	////pyra2->SetModelScale(Vector3(3, 20, 3));
	////pyra2->SetTransform(Matrix4::Translation(Vector3(50, 10, 0)));
	////pyra1->AddChild(pyra2);
	//////AddChild(pyra1);

	//SceneNode*	ship2 = new SceneNode(pyra, Vector4(1, 1, 0, 1));
	//ship2->SetModelScale(Vector3(10, 10, 10));
	//ship2->SetTransform(Matrix4::Translation(Vector3(15, 150, 1500)));
	////body->AddChild(pyra1);
	//pyra1->AddChild(ship2);

	//SceneNode*	ship3 = new SceneNode(pyra, Vector4(1, 1, 0, 1));
	//ship3->SetModelScale(Vector3(150, 150, 150));
	//ship3->SetTransform(Matrix4::Translation(Vector3(200, 200, 20)));
	////body->AddChild(pyra1);
	////pyra1->AddChild(ship3);

}

void CubeRobot::Update(float msec) {
	//transform = transform * Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));
	//transform = transform * Matrix4::Rotation(msec / 90.0f, Vector3(0, 0, 1));
	//transform = transform * Matrix4::Rotation(msec/ 90.0f, Vector3(0, 1, 0));
	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(0, 1, 0)));
	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));
	//rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));


	SceneNode::Update(msec);

}


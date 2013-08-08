#pragma once

#include "SceneNode.h"
//#include "../jni/OBJMesh.h"



class CubeRobot : public SceneNode {
public:
	CubeRobot(void);
	~CubeRobot(void){};
	virtual void Update(float msec);

	static void CreateCube() {

		//Removing the mesh loader for now. 
		/*OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh("../../Meshes/cube.obj");
		cube = m;*/
		cube = Mesh::GenerateTriangle();


	}
	static void DeleteCube() {delete cube;}


protected:
	static Mesh*	cube;

	SceneNode*		head;
	SceneNode*		leftArm;
	SceneNode*		rightArm;

};


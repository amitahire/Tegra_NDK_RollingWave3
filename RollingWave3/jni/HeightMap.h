#pragma once


#include "Common.h"
#include "Mesh.h"
#include "OctTree (2).h"
#include "Physics\PhysicsSystem.h"


#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHT_X 16.0f
#define HEIGHT_Z 16.0f
#define HEIGHT_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f/16.0f
#define HEIGHTMAP_TEX_Z 1.0f/16.0f

class HeightMap : public Mesh {

public:
	HeightMap(const char* filename);
	~HeightMap(void){};

private:

	PhysicsNode** particleNode;

	vector<PhysicsNode*> particleList;

	OctTree* mapTree;

	PhysicsNode* physicsBound;

};



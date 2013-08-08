#include "HeightMap.h"

HeightMap::HeightMap(const char* filename) {

	///////////////////For HM collision _ Octree//////////////
	//////////////////////////////////////////////////////////
	////Temp Variable
	//Vector3 maxXYZ = Vector3(0);
	//Vector3 minXYZ = Vector3(0);
	//
	////Physics Particle Construction
	//particleNode = new PhysicsNode*[RAW_WIDTH];
	//for(int i = 0; i < RAW_WIDTH;i++){
	//	particleNode[i] = new PhysicsNode[RAW_HEIGHT];
	//	for(int j = 0; j < RAW_HEIGHT;j++){
	//		//particleNode[i][j].CreateAsCollisionTypeSphere();
	//		particleNode[i][j].CreateAsCollisionTypePlane();
	//	}
	//}
	
	NvFile *file = NvFOpen(filename);
	if(!file) {
		LOGD("HeightMap.cpp - Unable to open file.");
		return;
	}

	//Size of the file.
	size_t size = NvFSize(file);

	numVertices = RAW_WIDTH*RAW_HEIGHT;
	numIndices = (RAW_WIDTH-1)*(RAW_HEIGHT-1)*6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	unsigned char* data = new unsigned char[numVertices];
	NvFRead(data, numVertices*sizeof(unsigned char), size, file);
	NvFClose(file);

	// Adding some
	float Min = 1096162876;
	float Max = FLT_MIN;
	//int flag = 0;
	// Min = 0 --- Max = 66048
	for(int x = 0; x < RAW_WIDTH; ++x){
		for(int z = 0; z < RAW_HEIGHT; ++z) {
			int offset = (x * RAW_WIDTH) +z;

			//if(flag == 0){Min = offset;flag++;}


			float height =  data[offset] * HEIGHT_Y;

			vertices[offset] = Vector3(x * HEIGHT_X, data[offset] * HEIGHT_Y, z * HEIGHT_Z);
			//cout << height << endl;
			//if(height < 90){
				//colour[offset] = 
			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);

			if (height > Max)
				Max = height;
			
			if (height < Min)	
				Min = height;


			////Setup ParticleNode
			////particleNode[x][z].GetCollisionSphere()->position = vertices[offset];
			////*set collision plane stuff after surface normal*/
			//particleNode[x][z].SetPosition(vertices[offset]);
			//particleNode[x][z].GetAABB()->position = vertices[offset];
			//particleNode[x][z].GetAABB()->halfDimension = Vector3(8.0f, 8.0f, 8.0f);
			//particleNode[x][z].SetCollisionType(COLLISIONTYPE_HEIGHTMAP);
			//particleNode[x][z].type = HEIGHTMAP_ENTITY;
			////*really heavy so inverseMass is zero*/
			//particleNode[x][z].SetInverseMass(0.0f);

			////particleNode[x][z].GetCollisionSphere()->radius = 50.0f;
			//particleNode[x][z].SetAtRestBool(true);
			////if(offset%HEIGHTMAP_COLLSION_SEPARATION == 0){
			//particleList.push_back(&particleNode[x][z]);
			//

			////Max and Min XYZ Detection
			//if(x == 0 && z == 0){
			//	maxXYZ = particleNode[x][z].GetPosition();
			//	minXYZ = particleNode[x][z].GetPosition();
			//} else {
			//	if(maxXYZ.x < particleNode[x][z].GetPosition().x){ maxXYZ.x = particleNode[x][z].GetPosition().x; }
			//	if(maxXYZ.y < particleNode[x][z].GetPosition().y){ maxXYZ.y = particleNode[x][z].GetPosition().y; }
			//	if(maxXYZ.z < particleNode[x][z].GetPosition().z){ maxXYZ.z = particleNode[x][z].GetPosition().z; }
			//	if(minXYZ.x > particleNode[x][z].GetPosition().x){ minXYZ.x = particleNode[x][z].GetPosition().x; }
			//	if(minXYZ.y > particleNode[x][z].GetPosition().y){ minXYZ.y = particleNode[x][z].GetPosition().y; }
			//	if(minXYZ.z > particleNode[x][z].GetPosition().z){ minXYZ.z = particleNode[x][z].GetPosition().z; }
			//
			//}

		}

	}

	//cout << "MIN : " << Min << "\t MAX: " << Max << endl;

	delete data;

	//Indices.
	numIndices = 0;

	for(int x = 0; x < RAW_WIDTH-1; ++x){
		for(int z =0; z < RAW_HEIGHT-1; ++z){
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x+1) * (RAW_WIDTH)) + z;
			int c = ((x+1) * (RAW_WIDTH)) + (z+1);
			int d = (x * (RAW_WIDTH)) + (z+1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;

		}
	}
	//Tutorial 11 A - Lighting
	GenerateNormals();
	//GenerateTangents();
	BufferData();

	////Custom Normal Calculation for Collision Impulse
	//for (unsigned int i = 0; i < RAW_WIDTH -1; ++ i ) {
	//	for (unsigned int j = 0; j < RAW_HEIGHT-1 ; ++ j ) {
	//		unsigned int origin = (i * ( RAW_WIDTH )) + j ; //Origin
	//		unsigned int top = ((i +1) * ( RAW_WIDTH )) + j ; //Top				
	//		unsigned int bottom = ((i -1) * ( RAW_WIDTH )) + j ; //Bottom
	//		unsigned int left = ( i * ( RAW_WIDTH )) + ( j +1); //Left
	//		unsigned int right = ( i * ( RAW_WIDTH )) + ( j -1); //Right
	//		Vector3 surfaceNormal = normals[origin];
	//		
	//		if(top >= 0 && top < numVertices){ surfaceNormal += normals[top];}
	//		if(bottom >= 0 && bottom < numVertices){ surfaceNormal += normals[bottom];}
	//		if(left >= 0 && left < numVertices){ surfaceNormal += normals[left]; }
	//		if(right >= 0 && right < numVertices){ surfaceNormal += normals[right];}
	//		surfaceNormal.Normalise();
	//		
	//		//Set custom facing normal for particle
	//		particleNode[i][j].SetUseSelfNormalBool(true);
	//		particleNode[i][j].SetCollisionNormal(surfaceNormal);	
	//		particleNode[i][j].GetCollisionPlane()->SetNormal(surfaceNormal);
	//		particleNode[i][j].GetCollisionPlane()->SetDistance((-1)*Vector3::Dot(particleNode[i][j].GetPosition(), surfaceNormal));
	//	}
	//}


	////HeightMap Done Generation,Structure particle node into Oct Tree
	//mapTree = new OctTree(Vector3(minXYZ+((maxXYZ-minXYZ)/2)),Vector3((maxXYZ-minXYZ)/2),1);
	//for(vector<PhysicsNode*>::iterator i = particleList.begin(); i != particleList.end(); ++i) { mapTree->add((*i));}

	////Create Bounding Box For HeightMap and Set Collision Type]
	//physicsBound = new PhysicsNode();	
	////minXYZ  = minXYZ *5000;


	//physicsBound->GetAABB()->position = Vector3(minXYZ+((maxXYZ-minXYZ)/2));
	//physicsBound->GetAABB()->halfDimension = Vector3((maxXYZ-minXYZ)/2);
	//physicsBound->GetAABB()->ListPtr = mapTree;
	//physicsBound->SetAtRestBool(true);
	//physicsBound->type = HEIGHTMAP_BOUND_ENTITY;
	//physicsBound->SetCollisionType(COLLISIONTYPE_NULL);

	////Add into Physics System
	//PhysicsSystem::GetPhysicsSystem().AddNode(physicsBound);



}





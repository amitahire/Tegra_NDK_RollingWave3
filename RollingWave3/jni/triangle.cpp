#include "triangle.h"

///////////////////////////////////////////////////////////////////////////////
//DEFINATIONS

#define TRIANGLE_SHADER "triangle"
#define VERTEX_SHADER_BUFFER 0
#define COLOUR_SHADER_BUFFER 1

using namespace std;
///////////////////////////////////////////////////////////////////////////////
//PUBLIC METHODS
triangle* triangle::appInstance = NULL;
PhysicsSystem* triangle::physicsInstance = NULL;

#define min(a,b)    (((a) < (b)) ? (a) : (b))


/**
 *	Constructor
 */
triangle::triangle(void)
{
	m_bInitialized = false;
	appInstance = this;
	physicsInstance = NULL;
	camera = new Camera();
	//Default Cam Position.
	camera->SetPosition(Vector3(2065, 2170, 1897));
	camera->SetYaw(1.05);
	camera->SetPitch(-26.59);
	viewMatrix = camera->BuildViewMatrix();
	

	renderCounter	= 0.0f;
	physicsCounter	= 0.0f;
	renderTime = 0.0f;

	//Init PhysicsSystem.
	PhysicsSystem::Initialise();
	//PhysicsSystem::GetPhysicsSystem().SetCamera(camera);

	root			= new SceneNode();
	FPS				= 0;


}

/**
 * Destructor
 */
triangle::~triangle(void)
{
	
	//LOGD("ERROR:\tdestructor called while not cleaned up properly.");
	delete root;
	delete ico;
	delete heightMap;
	delete light;
	//Delete sceneNode - Check where.

}

/**
 * A Simple Load file function for Android.
 **/
char* load_fileAndroid(const char* file)
{
    NvFile *f = NvFOpen(file);
    size_t size = NvFSize(f);
    char *buffer = new char[size+1];
    NvFRead(buffer, 1, size, f);
    NvFClose(f);
    buffer[size] = '\0';

    return buffer;
}


/**
 * Initializer.
 */
bool triangle::init()
{

	if(m_bInitialized)
	{
		LOGD("ERROR:\tinit method called while not cleaned up properly.");
		return false;
	}

	//INITIALIZE VALUES
	if(!(resetValues()))
		return false;

	//CREATE GEOMETRY
    if(!(initMeshesGL()))
    	return false;

    //CREATE GPU PROGRAMS
    if(!(initShadersGL())) // - DONE.
    	return false;

    //SETTING OPENGL STATE
	if(!(initStateGL()))
		return false;

	m_bInitialized = true;

    return true;
	
}

/**
 * SET YOUR INITIAL VALUES HERE
 */
bool triangle::resetValues()
{
	m_bInitialized = true;
	SwitchToOrthographic(45);
	//SwitchToPerspective();

	return true;
}


/**
 * // Init Texture.
 */
bool triangle::initTexturesGL(void)
{
	//bool tex0 = createHHDDSTextureGL(m_uivTextures[0], 0, GL_TEXTURE0,
	//								 true, true, COLOR_TEXTURE_FILENAME);
	//bool tex1 = createNormalMapGL(m_uivTextures[1], 1, GL_TEXTURE1,
	//							  m_fHeightScale, HEIGHTMAP_TEXTURE_FILENAME);
	//bool tex2 = createHHDDSTextureGL(m_uivTextures[2], 2, GL_TEXTURE2,
	//								 true, true, CITYLIGHTS_TEXTURE_FILENAME);
	//bool tex3 = createHHDDSTextureGL(m_uivTextures[3], 3, GL_TEXTURE3,
	//								 true, true, CLOUDS_TEXTURE_FILENAME);
	//return tex0 && tex1 && tex2 && tex3;

	//Textures for Triangle.
	//char* texBuffer[2]; // Change the value to number of texture. WONT BE NECESSERY THOUGH.
	//int bufferSize;

	//texBuffer[0]	= load_fileAndroid("side4.png"); //Use "filename" - to get different file with buffer and sprintf - LATER.

	//bufferSize		= sizeof(texBuffer[0]);

	//tri->SetTexture(SOIL_load_OGL_texture_from_memory((unsigned char *)texBuffer[0], bufferSize, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	//
	////tri->SetTexture(SOIL_load_OGL_texture("Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	//
	////NvS32 swidth = 1;
 ////   NvS32 sheight = 1;
	////tri->SetTexture( NvCreateTextureFromDDSEx("zappy_android.dds",
	////									NV_FALSE,
	////									NV_TRUE,
	////									&swidth, &sheight, NV_NULL, NV_NULL));

	////LOGD(">>> WIDTH : %f", (float)swidth);
	////LOGD(">>> HEIGHT : %f", (float)sheight);

	//if(!tri->GetTexture()){
	//	return false;
	//}

	//Binding.
	glBindTexture(GL_TEXTURE_2D, tri->GetTexture());
	//Repeating.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);


	return true;

}

/**
 * Shader init.
 */

bool triangle::initShadersGL(void)
{

	simple_shader	= nv_load_program(SIMPLE_SHADER);
	shaderTEX		= nv_load_program(TEX_SHADER);
	shaderHeightMap = nv_load_program(HEIGHTMAP_SHADER);
	shaderSceneNode = nv_load_program(SCENENODE_SHADER);
	shaderLight		= nv_load_program(LIGHT_SHADER);
	shaderMainBall  = nv_load_program(MAINBALL_SHADER);
	shaderQuad		= nv_load_program(QUAD_SHADER);
	shaderBall		= nv_load_program(BALL_SHADER);
	
	LOGD("DONE - Compiling and binding.");
	return true;

}

/**
 *
 */
bool triangle::initMeshesGL(void)
{
	//TODO - Delete all the objects when done. Check all.

	tri = Mesh::GenerateTriangle();
	quad = Mesh::GenerateQuad();
	ico = MeshData();

	//HeightMap.
	//heightMap = new HeightMap("KPAX-Fin2.raw");
	//heightMap = new HeightMap("Level-RandomPerlin-5.raw");
	//heightMap = new HeightMap("Level-5-Theta.raw");
		
	/////////////Seperaete function maybe.////////////////
	allEntities.push_back(BuildMainBall(Vector3(1065, 870, 400)));
	allEntities.push_back(BuildBall(Vector3(1115, 900, -400)));
	allEntities.push_back(BuildBall(Vector3(0, 100, 0)));

	//Spawning many balls.
	for ( int i = 0 ; i < 50; i++) {

		allEntities.push_back(BuildBall(Vector3(rand() % 2000, rand() % 2000, rand() % 2000)));
	}

	//Setting main ball.
	PhysicsSystem::GetPhysicsSystem().SetMainBall();
	//Plane to roll on.
	allEntities.push_back(BuildQuadPlane(Vector3(1, -100, 1)));
	//////////////////////////////////////////////////////


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

/**
 *Setting light here.
 **/
bool triangle::initLight(void){

	light	= new Light(Vector3(0.0f, 100.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 300.0f);
	glUseProgram(shaderLight);
	Vector3 pos = light->GetPosition();
	Vector4 col = light->GetColour();
	float radius = light->GetRadius();

	glUniform3fv(glGetUniformLocation(shaderLight, "lightPos")   ,1,(float*)&pos);
	glUniform4fv(glGetUniformLocation(shaderLight, "lightColour"),1,(float*)&col);
	glUniform1f(glGetUniformLocation (shaderLight, "lightRadius"),(float)radius);

	glUniform1i(glGetUniformLocation(shaderLight, "diffuseTex"), 0);

	glUseProgram(0);

	return true;

}

/**
 *Setting some intitial states. 
 */
bool triangle::initStateGL(void)
{

	//Debug info about the device. Disabling now.
	//LOGD("INIT:\tGL_VENDOR: %s",   glGetString(GL_VENDOR));
	//LOGD("INIT:\tGL_VERSION: %s",  glGetString(GL_VERSION));
	//LOGD("INIT:\tGL_RENDERER: %s", glGetString(GL_RENDERER));
	//LOGD("INIT:\tGL_EXTENSIONS:");
	
	const GLubyte* ext = glGetString(GL_EXTENSIONS);
	char *extTok = strdup((const char*)ext);

	extTok = strtok (extTok," ");
	while (extTok != NULL)
	{
		LOGD("INIT:\t\t%s", extTok);
		extTok = strtok (NULL, " ");
	}

	glEnable(GL_DEPTH_TEST); //depth function is GL_LESS.
	glEnable(GL_CULL_FACE);  //cull face is GL_BACK.
	glEnable(GL_BLEND);      //Blending on.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f,0.6f,1.0f,1.0f);	

	return true;
}


/**
 * Render - if everything is intialized.
 */
bool triangle::render(const float current_time)
{
	if(m_bInitialized)
	{
	
		return renderGL(current_time);
	}

	LOGD("ERROR:\trender method called while not initialized properly.");
	return false;
}

/**
 * Render - Draw Things.
 */
bool triangle::renderGL(const float current_time)
{
	// ******* Using the shaders created and log messges. NOTE - how to use it.******//
	//***********************Then we call the Draw() from the Mesh class. ***********//

	DrawIco();

	DrawLight();

	//DrawHeightMap();

	DrawQuad();

	DrawRootMain();


}

/*
**Scene Management.
*/
void	triangle::DrawRoot() {

	BuildNodeLists(root);
	//SortNodeLists();
	DrawNodes();

}


void	triangle::BuildNodeLists(SceneNode* from)	{

	Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(direction,direction));

	if(from->GetColour().w < 1.0f) 
	{
		transparentNodeList.push_back(from);
		if(from->GetLightPtr() != NULL) {
			lightList.push_back(from);
		} 
	}
	else{
		nodeList.push_back(from);
	}
	

	for(vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}


void	triangle::DrawNodes()	 {


	for(vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i ) {
		DrawNode((*i));
	}

	for(vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i ) {
		DrawNode((*i));
	}

}

void	triangle::DrawNode(SceneNode* n)	
{

	if(n->isMainBall == true){

		glUseProgram(shaderMainBall);
	} else {
		
		glUseProgram(shaderBall);

	}


	if(n->GetMesh()) {
		Matrix4 transform	= n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		
		Vector4 col = n->GetColour();
		//LOGD("%f : %f : %f : %f", n->GetColour().x, n->GetColour().y, n->GetColour().z, n->GetColour().w);
		//LOGD("%f, %f, %f :", (float)transform.GetPositionVector().x, (float)transform.GetPositionVector().y, (float)transform.GetPositionVector().z);

		glUniformMatrix4fv(glGetUniformLocation(shaderMainBall, "modelMatrix"), 1, false, (float*)&transform);
		glUniformMatrix4fv(glGetUniformLocation(shaderMainBall, "viewMatrix") ,	1,false, (float*)&viewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shaderMainBall, "projMatrix") ,	1,false, (float*)&projMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shaderMainBall, "textureMatrix")  ,1,false, (float*)&textureMatrix);

		glUniform4fv(glGetUniformLocation(shaderMainBall, "nodeColour"), 1, (float*)&col);
		//glUniform1i(glGetUniformLocation(shaderSceneNode, "useTexture"), (int)n->GetMesh()->GetTexture());
		glEnable(GL_DEPTH_TEST);
		n->Draw();
	}
	  
	glUseProgram(0);

}

void	triangle::ClearNodeLists()	{
	transparentNodeList.clear();
	nodeList.clear();
	lightList.clear();
}

void	triangle::AddNode(SceneNode* n) {
	root->AddChild(n);
}

void	triangle::RemoveNode(SceneNode* n) {
	root->RemoveChild(n);
}


void triangle::SwitchToPerspective() {

	projMatrix = Matrix4::Orthographic(-1.0f,10000.0f,(float)this->getWinHeight()/2.0f,-(float)getWinHeight()/2.0f,(float)getWinWidth()/2.0f,-(float)getWinWidth()/2.0f);
	//projMatrix = Matrix4::Orthographic(-1.0f,10000.0f,(float)1280/2.0f,(float)-1280/2.0f,(float)800/2.0f,(float)-800/2.0f);
}



void triangle::SwitchToOrthographic(float fov) {

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)getWinHeight()/(float)getWinWidth(), fov);
	//projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)1280/(float)800, fov);
	
}

void triangle::UpdateScene(float msec) {

	
	FPS				= 1000/msec;
	viewMatrix		= camera->BuildViewMatrix();

	//////////////////////////Updating Camera - Follow the ball.//////////////////////////
	//if(toggleCamera == false) {

	//camera->SetPosition(Vector3(GetPhysicsSystem().GetPointerToMainBall()->GetPosition().x - 700 * camera->Forward().x,
	//					GetPhysicsSystem().GetPointerToMainBall()->GetPosition().y - 700 * camera->Forward().y,
	//					GetPhysicsSystem().GetPointerToMainBall()->GetPosition().z - 700 * camera->Forward().z));

	////camera->SetPitch(min (pitch, -5.0f));

	//PhysicsSystem::GetPhysicsSystem().SetCamera(camera);
	//}

	/////////////////////////////////////////////////////////////////


	root->Update(msec);
	PhysicsSystem::GetPhysicsSystem().Update(msec);

}


/**
 * Scene Update - Possibly do the updates and re-render here.
 **/
void triangle::UpdateCore(float msec) {

	renderCounter	-= msec; 
	physicsCounter	+= msec;
	renderTime += msec;

	//Update our rendering logic
	if(renderCounter <= 0.0f) {	
	
		triangle::UpdateScene(renderTime);
		triangle::renderGL();
		renderCounter += (1000.0f / (float)RENDER_HZ);
		renderTime = 0.0f;
	}

	while(physicsCounter >= 0.0f) {
		physicsCounter -= PHYSICS_TIMESTEP;
		PhysicsSystem::GetPhysicsSystem().Update(PHYSICS_TIMESTEP);

	}

}

/**
 * Set Deafualt Attributes for the shader.
 **/

void triangle::SetDefaultAttributes(GLuint shader_name) {

	glUniformMatrix4fv(glGetUniformLocation(shader_name, "modelMatrix"),	1,false, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_name, "viewMatrix") ,	1,false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_name, "projMatrix") ,	1,false, (float*)&projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_name, "textureMatrix")  ,1,false, (float*)&textureMatrix);


}

/**
 * Build Main Ball and Connect to the system.
 **/
GameEntity* triangle::BuildMainBall(Vector3 pos){

	SceneNode* s = new SceneNode(ico);
	PhysicsNode* p = new PhysicsNode(COLLISIONTYPE_SPHERE);
	float temp_radius = 40;
	//Physics Properties.
	p->SetPosition(pos);
	p->type = MAINBALL_ENTITY;
	p->cType = COLLISIONTYPE_SPHERE;
	p->SetRadius(temp_radius); // Check this- Maybe be wrong.
	p->SetMass(30);
	Matrix4 temp = Matrix4();
	temp.ToZero();
	temp.SetScalingVector(Vector3(2.5f*(1/(p->GetInverseMass()* p->GetRadius() * p->GetRadius())), 2.5f*(1/(p->GetInverseMass()* p->GetRadius() * p->GetRadius())), 2.5f*(1/(p->GetInverseMass()* p->GetRadius() * p->GetRadius()))));
	p->SetInvInertia(temp);
	p->SetAtRestBool(false);
	p->SetTorque(10.8);
	p->SetAngularVelocity(Vector3(30,0,0));
	p->SetMoveableBool(true);
	
	//Collision Properties.
	//p->GetCollisionSphere()->position = pos;
	//p->GetCollisionSphere()->radius = 10.0;
	//Which radius are we talking about? - CHECK
	p->GetAABB()->SetHalfDimension(Vector3(p->GetRadius(), p->GetRadius(), p->GetRadius()));
	p->GetAABB()->SetPosition(pos);
	
	p->GetCollisionSphere()->SetPosition(pos);
	//p->GetCollisionSphere()->SetRadius(100);
	p->GetCollisionSphere()->SetRadius(temp_radius * 0.3);
	//SceneNode Properties.
	s->boundingRadius = (temp_radius - 10);
	s->colour = Vector4(0.0, 1.0, 0.0, 1.0);
	s->isMainBallChild = true;
	s->SetModelScale(Vector3(temp_radius * 2,temp_radius * 2,temp_radius * 2));
	s->SetColour(Vector4(1.0f, 0.0, 0.0, 1.0));
	s->isMainBall = true;
	
	GameEntity *g = new GameEntity(s, p);

	g->ConnectToSystems();

	return g;
}

/**
 * Build Sticky balls and Connect to the system.
 **/
GameEntity* triangle::BuildBall(Vector3 pos){

	SceneNode* s = new SceneNode(ico);
	PhysicsNode* p = new PhysicsNode(COLLISIONTYPE_SPHERE);

	//Physics Properties.
	p->SetPosition(pos);
	p->type = STICKING_ENTITY;
	p->cType = COLLISIONTYPE_SPHERE;
	p->SetRadius(100); // Check this- Maybe be wrong.
	Matrix4 temp = Matrix4();
	temp.ToZero();
	temp.SetScalingVector(Vector3(2.5f*(1/(p->GetInverseMass()* p->GetRadius() * p->GetRadius())), 2.5f*(1/(p->GetInverseMass()* p->GetRadius() * p->GetRadius())), 2.5f*(1/(p->GetInverseMass()* p->GetRadius() * p->GetRadius()))));
	p->SetInvInertia(temp);
	p->SetAtRestBool(false);
	p->SetIsStickingBool(false);
	p->SetMoveableBool(true);
	
	p->SetAngularVelocity(Vector3(10, 10, 10));
	//Collision Properties.
	p->GetAABB()->SetHalfDimension(Vector3(p->GetRadius(), p->GetRadius(), p->GetRadius()));
	p->GetAABB()->SetPosition(pos);
	
	p->GetCollisionSphere()->SetPosition(pos);
	p->GetCollisionSphere()->SetRadius(100);

	//SceneNode Properties.
	s->boundingRadius = 20;
	s->SetColour(Vector4(0.0, 1.0, 0.0, 1.0));
	//s->isMainBallChild = true;
	s->SetModelScale(Vector3(30,30,30));
	
	
	
	GameEntity *g = new GameEntity(s, p);

	g->ConnectToSystems();

	return g;
}

/**
 * Build a Quad and plane for surfaces.
 **/
GameEntity* triangle::BuildQuadPlane(Vector3 pos) {

	SceneNode* s = new SceneNode(quad);
	PhysicsNode* p = new PhysicsNode(COLLISIONTYPE_PLANE);

	//Physical Properties.
	p->SetPosition(pos);
	p->type = SURFACE_ENTITY;
	p->SetAtRestBool(false);
	p->SetIsStickingBool(false);
	p->GetCollisionPlane()->SetNormal(Vector3(0,-1,0));
	p->GetCollisionPlane()->SetDistance(0);
	p->boundingBox->position = pos;
	p->boundingBox->halfDimension = Vector3(9999999,9999999,9999999);
	p->SetCollisionNormal(Vector3(0, -1, 0));

	p->SetCollisionSphere(new CollisionSphere(pos, 100));

	
	//SceneNode Properties.
	//s->SetModelScale(Vector3(1000,1000,1000));
	s->SetColour(Vector4(0,0,1,1));
	s->boundingRadius = 1000;


	

	GameEntity *g = new GameEntity(s, p);

	g->ConnectToSystems();

	return g;

}


/**
 * Update Objects in the world.
**/
void triangle::UpdateGame(float msec) {

	//Add stuff to control the ball here based on the accelerometer reading.
	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {

		(*i)->Update(msec);
	}

	this->ClearNodeLists();
}

/**
 * Draw Ico.
 **/

void triangle::DrawIco(){

	///////////////////////////////////////////////////////////////////////////////
	// ICO/BALL ///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT);
	
	
	glUseProgram(shaderMainBall);

	Vector3 ballPos = position;
	ballPos.x -= (1.0f);
	ballPos.y += (50.0f);
	ballPos.z -= (1.0f);

	modelMatrix =	Matrix4::Translation(ballPos ) *	
					Matrix4::Rotation(90, Vector3(0,1,0)) *
					Matrix4::Scale(Vector3(30,30,30));

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)1280/(float)800, 45.0f);
	//projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)getWinHeight()/(float)getWinWidth(), 45.0f);
	SetDefaultAttributes(shaderMainBall);
	glUniform1i(glGetUniformLocation(shaderMainBall, "diffuseTex"), 0);
	//glDisable(GL_CULL_FACE);
	ico->Draw();
	//glEnable(GL_CULL_FACE);
	glUseProgram(0);


}

/**
 * Draw Lights.
 **/
void triangle::DrawLight() {

	//Light init.
	if(!initLight()){
		LOGD("Light not initialised");
	}
}

/**
 * Draw HeightMaps.
 **/
void triangle::DrawHeightMap() {


	//////////////////////////////////////////////////////////////////////////
	// HeightMap. ////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	glUseProgram(shaderHeightMap);

	Vector3 HMPos = position;
	HMPos.x = -1000.0f;
	HMPos.y = +10.0f;
	HMPos.z = -1000.0f;

	modelMatrix =	Matrix4::Translation(HMPos) *	
					Matrix4::Rotation(90, Vector3(0,1,0));

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)1280/(float)800, 45.0f);
	//projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)getWinHeight()/(float)getWinWidth(), 45.0f);

	SetDefaultAttributes(shaderHeightMap);

	glUniform1i(glGetUniformLocation(shaderHeightMap, "diffuseTex"), 0);
	
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	heightMap->Draw();
	glEnable(GL_CULL_FACE);


	glUseProgram(0);
}

/**
 * Draw Quad.
 **/
void triangle::DrawQuad() {

	/////////////////////////////////////////////////////////////////////////////
	//// Quad ///////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(shaderQuad);
	
	Vector3 tempPos = position;
	tempPos.x -= (1.0f);
	tempPos.y = -10.0f;
	tempPos.z -= (1.0f);

	modelMatrix =	Matrix4::Translation(tempPos) *	
					Matrix4::Rotation(90, Vector3(1,0,0)) *
					Matrix4::Scale(Vector3(4000,4000,4000));

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)1280/(float)800, 45.0f);
	//projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)getWinHeight()/(float)getWinWidth(), 45.0f);

	SetDefaultAttributes(shaderQuad);
	//camera->SetPosition(Vector3(-1,-1,200));
	glDisable(GL_CULL_FACE);
	quad->Draw();
	glEnable(GL_CULL_FACE);
	glUseProgram(0);
}

void triangle::DrawRootMain() {

	//glDisable(GL_CULL_FACE);
	DrawRoot();
	//glEnable(GL_CULL_FACE);
}


/**
*  Mesh Data - For the ball - Maybe add more objects later.
**/

Mesh* triangle::MeshData() {

unsigned int vertex_count[]={240};

struct vertex_struct {
	float x,y,z;
	float nx,ny,nz;
	float u,v;
};

struct vertex_struct Mvertices[]={
	/* ICO: 240 vertices */
	{0.262929f, -0.525738f, 0.809005f, 0.471318f, -0.661688f, 0.583119f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, 0.309004f, 0.471318f, -0.661688f, 0.583119f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, 0.525713f, 0.471318f, -0.661688f, 0.583119f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, 0.309004f, 0.187594f, -0.794659f, 0.577344f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, 0.809005f, 0.187594f, -0.794659f, 0.577344f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, 0.499988f, 0.187594f, -0.794659f, 0.577344f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, 0.499988f, -0.038548f, -0.661688f, 0.748788f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, 0.809005f, -0.038548f, -0.661688f, 0.748788f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, 0.850633f, -0.038548f, -0.661688f, 0.748788f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, 0.499988f, 0.102381f, -0.943523f, 0.315091f, 0.000000f, 0.000000f},
	{0.000060f, -1.000000f, -0.000007f, 0.102381f, -0.943523f, 0.315091f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, 0.309004f, 0.102381f, -0.943523f, 0.315091f, 0.000000f, 0.000000f},
	{0.850708f, -0.525736f, -0.000007f, 0.700227f, -0.661689f, 0.268049f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, 0.525713f, 0.700227f, -0.661689f, 0.268049f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, 0.309004f, 0.700227f, -0.661689f, 0.268049f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, 0.309004f, 0.607059f, -0.794656f, 0.000000f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, -0.309018f, 0.607059f, -0.794656f, 0.000000f, 0.000000f, 0.000000f},
	{0.850708f, -0.525736f, -0.000007f, 0.607059f, -0.794656f, 0.000000f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, -0.525727f, 0.700227f, -0.661689f, -0.268049f, 0.000000f, 0.000000f},
	{0.850708f, -0.525736f, -0.000007f, 0.700227f, -0.661689f, -0.268049f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, -0.309018f, 0.700227f, -0.661689f, -0.268049f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, 0.309004f, 0.331305f, -0.943524f, 0.000000f, 0.000000f, 0.000000f},
	{0.000060f, -1.000000f, -0.000007f, 0.331305f, -0.943524f, 0.000000f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, -0.309018f, 0.331305f, -0.943524f, 0.000000f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, 0.499990f, -0.408938f, -0.661687f, 0.628443f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, 0.499988f, -0.408938f, -0.661687f, 0.628443f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, 0.850633f, -0.408938f, -0.661687f, 0.628443f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, 0.499988f, -0.491119f, -0.794658f, 0.356822f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, 0.499990f, -0.491119f, -0.794658f, 0.356822f, 0.000000f, 0.000000f},
	{-0.525670f, -0.850652f, -0.000007f, -0.491119f, -0.794658f, 0.356822f, 0.000000f, 0.000000f},
	{-0.894365f, -0.447215f, -0.000007f, -0.724045f, -0.661693f, 0.194734f, 0.000000f, 0.000000f},
	{-0.525670f, -0.850652f, -0.000007f, -0.724045f, -0.661693f, 0.194734f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, 0.499990f, -0.724045f, -0.661693f, 0.194734f, 0.000000f, 0.000000f},
	{-0.525670f, -0.850652f, -0.000007f, -0.268034f, -0.943523f, 0.194737f, 0.000000f, 0.000000f},
	{0.000060f, -1.000000f, -0.000007f, -0.268034f, -0.943523f, 0.194737f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, 0.499988f, -0.268034f, -0.943523f, 0.194737f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, -0.500004f, -0.724045f, -0.661693f, -0.194734f, 0.000000f, 0.000000f},
	{-0.525670f, -0.850652f, -0.000007f, -0.724045f, -0.661693f, -0.194734f, 0.000000f, 0.000000f},
	{-0.894365f, -0.447215f, -0.000007f, -0.724045f, -0.661693f, -0.194734f, 0.000000f, 0.000000f},
	{-0.525670f, -0.850652f, -0.000007f, -0.491119f, -0.794658f, -0.356822f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, -0.500004f, -0.491119f, -0.794658f, -0.356822f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, -0.500002f, -0.491119f, -0.794658f, -0.356822f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, -0.850647f, -0.408938f, -0.661687f, -0.628443f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, -0.500002f, -0.408938f, -0.661687f, -0.628443f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, -0.500004f, -0.408938f, -0.661687f, -0.628443f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, -0.500002f, -0.268034f, -0.943523f, -0.194737f, 0.000000f, 0.000000f},
	{0.000060f, -1.000000f, -0.000007f, -0.268034f, -0.943523f, -0.194737f, 0.000000f, 0.000000f},
	{-0.525670f, -0.850652f, -0.000007f, -0.268034f, -0.943523f, -0.194737f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, -0.809019f, -0.038548f, -0.661687f, -0.748788f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, -0.500002f, -0.038548f, -0.661687f, -0.748788f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, -0.850647f, -0.038548f, -0.661687f, -0.748788f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, -0.500002f, 0.187594f, -0.794659f, -0.577344f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, -0.809019f, 0.187594f, -0.794659f, -0.577344f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, -0.309018f, 0.187594f, -0.794659f, -0.577344f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, -0.525727f, 0.471318f, -0.661688f, -0.583119f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, -0.309018f, 0.471318f, -0.661688f, -0.583119f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, -0.809019f, 0.471318f, -0.661688f, -0.583119f, 0.000000f, 0.000000f},
	{0.425382f, -0.850654f, -0.309018f, 0.102381f, -0.943523f, -0.315091f, 0.000000f, 0.000000f},
	{0.000060f, -1.000000f, -0.000007f, 0.102381f, -0.943523f, -0.315091f, 0.000000f, 0.000000f},
	{-0.162396f, -0.850654f, -0.500002f, 0.102381f, -0.943523f, -0.315091f, 0.000000f, 0.000000f},
	{0.951118f, 0.000000f, 0.309006f, 0.904981f, -0.330393f, 0.268049f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, 0.525713f, 0.904981f, -0.330393f, 0.268049f, 0.000000f, 0.000000f},
	{0.850708f, -0.525736f, -0.000007f, 0.904981f, -0.330393f, 0.268049f, 0.000000f, 0.000000f},
	{0.850708f, -0.525736f, -0.000007f, 0.982246f, -0.187599f, 0.000000f, 0.000000f, 0.000000f},
	{0.951118f, -0.000000f, -0.309020f, 0.982246f, -0.187599f, 0.000000f, 0.000000f, 0.000000f},
	{0.951118f, 0.000000f, 0.309006f, 0.982246f, -0.187599f, 0.000000f, 0.000000f, 0.000000f},
	{0.894485f, 0.447215f, -0.000007f, 0.992077f, 0.125631f, -0.000000f, 0.000000f, 0.000000f},
	{0.951118f, 0.000000f, 0.309006f, 0.992077f, 0.125631f, -0.000000f, 0.000000f, 0.000000f},
	{0.951118f, -0.000000f, -0.309020f, 0.992077f, 0.125631f, -0.000000f, 0.000000f, 0.000000f},
	{0.951118f, -0.000000f, -0.309020f, 0.904980f, -0.330393f, -0.268049f, 0.000000f, 0.000000f},
	{0.850708f, -0.525736f, -0.000007f, 0.904980f, -0.330393f, -0.268049f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, -0.525727f, 0.904980f, -0.330393f, -0.268049f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, 0.850633f, 0.024725f, -0.330395f, 0.943519f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, 0.809005f, 0.024725f, -0.330395f, 0.943519f, 0.000000f, 0.000000f},
	{0.000060f, 0.000000f, 0.999993f, 0.024725f, -0.330395f, 0.943519f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, 0.809005f, 0.303531f, -0.187597f, 0.934172f, 0.000000f, 0.000000f},
	{0.587845f, 0.000000f, 0.809010f, 0.303531f, -0.187597f, 0.934172f, 0.000000f, 0.000000f},
	{0.000060f, 0.000000f, 0.999993f, 0.303531f, -0.187597f, 0.934172f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, 0.850633f, 0.306568f, 0.125651f, 0.943519f, 0.000000f, 0.000000f},
	{0.000060f, 0.000000f, 0.999993f, 0.306568f, 0.125651f, 0.943519f, 0.000000f, 0.000000f},
	{0.587845f, 0.000000f, 0.809010f, 0.306568f, 0.125651f, 0.943519f, 0.000000f, 0.000000f},
	{0.587845f, 0.000000f, 0.809010f, 0.534591f, -0.330395f, 0.777851f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, 0.809005f, 0.534591f, -0.330395f, 0.777851f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, 0.525713f, 0.534591f, -0.330395f, 0.777851f, 0.000000f, 0.000000f},
	{-0.950998f, 0.000000f, 0.309006f, -0.889698f, -0.330386f, 0.315091f, 0.000000f, 0.000000f},
	{-0.894365f, -0.447215f, -0.000007f, -0.889698f, -0.330386f, 0.315091f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, 0.499990f, -0.889698f, -0.330386f, 0.315091f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, 0.499990f, -0.794656f, -0.187595f, 0.577348f, 0.000000f, 0.000000f},
	{-0.587726f, 0.000000f, 0.809010f, -0.794656f, -0.187595f, 0.577348f, 0.000000f, 0.000000f},
	{-0.950998f, 0.000000f, 0.309006f, -0.794656f, -0.187595f, 0.577348f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, 0.525713f, -0.802607f, 0.125649f, 0.583125f, 0.000000f, 0.000000f},
	{-0.950998f, 0.000000f, 0.309006f, -0.802607f, 0.125649f, 0.583125f, 0.000000f, 0.000000f},
	{-0.587726f, 0.000000f, 0.809010f, -0.802607f, 0.125649f, 0.583125f, 0.000000f, 0.000000f},
	{-0.587726f, 0.000000f, 0.809010f, -0.574583f, -0.330397f, 0.748794f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, 0.499990f, -0.574583f, -0.330397f, 0.748794f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, 0.850633f, -0.574583f, -0.330397f, 0.748794f, 0.000000f, 0.000000f},
	{-0.587726f, -0.000000f, -0.809024f, -0.574583f, -0.330397f, -0.748794f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, -0.850647f, -0.574583f, -0.330397f, -0.748794f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, -0.500004f, -0.574583f, -0.330397f, -0.748794f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, -0.500004f, -0.794656f, -0.187595f, -0.577348f, 0.000000f, 0.000000f},
	{-0.950998f, -0.000000f, -0.309020f, -0.794656f, -0.187595f, -0.577348f, 0.000000f, 0.000000f},
	{-0.587726f, -0.000000f, -0.809024f, -0.794656f, -0.187595f, -0.577348f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, -0.525727f, -0.802607f, 0.125649f, -0.583125f, 0.000000f, 0.000000f},
	{-0.587726f, -0.000000f, -0.809024f, -0.802607f, 0.125649f, -0.583125f, 0.000000f, 0.000000f},
	{-0.950998f, -0.000000f, -0.309020f, -0.802607f, 0.125649f, -0.583125f, 0.000000f, 0.000000f},
	{-0.950998f, -0.000000f, -0.309020f, -0.889698f, -0.330386f, -0.315091f, 0.000000f, 0.000000f},
	{-0.688130f, -0.525736f, -0.500004f, -0.889698f, -0.330386f, -0.315091f, 0.000000f, 0.000000f},
	{-0.894365f, -0.447215f, -0.000007f, -0.889698f, -0.330386f, -0.315091f, 0.000000f, 0.000000f},
	{0.587845f, -0.000000f, -0.809024f, 0.534591f, -0.330395f, -0.777851f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, -0.525727f, 0.534591f, -0.330395f, -0.777851f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, -0.809019f, 0.534591f, -0.330395f, -0.777851f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, -0.809019f, 0.303531f, -0.187597f, -0.934172f, 0.000000f, 0.000000f},
	{0.000060f, -0.000000f, -1.000007f, 0.303531f, -0.187597f, -0.934172f, 0.000000f, 0.000000f},
	{0.587845f, -0.000000f, -0.809024f, 0.303531f, -0.187597f, -0.934172f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, -0.850647f, 0.306568f, 0.125651f, -0.943519f, 0.000000f, 0.000000f},
	{0.587845f, -0.000000f, -0.809024f, 0.306568f, 0.125651f, -0.943519f, 0.000000f, 0.000000f},
	{0.000060f, -0.000000f, -1.000007f, 0.306568f, 0.125651f, -0.943519f, 0.000000f, 0.000000f},
	{0.000060f, -0.000000f, -1.000007f, 0.024725f, -0.330395f, -0.943519f, 0.000000f, 0.000000f},
	{0.262929f, -0.525738f, -0.809019f, 0.024725f, -0.330395f, -0.943519f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, -0.850647f, 0.024725f, -0.330395f, -0.943519f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, 0.499990f, 0.889697f, 0.330387f, 0.315093f, 0.000000f, 0.000000f},
	{0.951118f, 0.000000f, 0.309006f, 0.889697f, 0.330387f, 0.315093f, 0.000000f, 0.000000f},
	{0.894485f, 0.447215f, -0.000007f, 0.889697f, 0.330387f, 0.315093f, 0.000000f, 0.000000f},
	{0.951118f, 0.000000f, 0.309006f, 0.794655f, 0.187596f, 0.577349f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, 0.499990f, 0.794655f, 0.187596f, 0.577349f, 0.000000f, 0.000000f},
	{0.587845f, 0.000000f, 0.809010f, 0.794655f, 0.187596f, 0.577349f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, 0.850633f, 0.574584f, 0.330397f, 0.748793f, 0.000000f, 0.000000f},
	{0.587845f, 0.000000f, 0.809010f, 0.574584f, 0.330397f, 0.748793f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, 0.499990f, 0.574584f, 0.330397f, 0.748793f, 0.000000f, 0.000000f},
	{0.587845f, 0.000000f, 0.809010f, 0.802606f, -0.125648f, 0.583126f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, 0.525713f, 0.802606f, -0.125648f, 0.583126f, 0.000000f, 0.000000f},
	{0.951118f, 0.000000f, 0.309006f, 0.802606f, -0.125648f, 0.583126f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, 0.809004f, -0.024727f, 0.330396f, 0.943518f, 0.000000f, 0.000000f},
	{0.000060f, 0.000000f, 0.999993f, -0.024727f, 0.330396f, 0.943518f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, 0.850633f, -0.024727f, 0.330396f, 0.943518f, 0.000000f, 0.000000f},
	{0.000060f, 0.000000f, 0.999993f, -0.303530f, 0.187599f, 0.934171f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, 0.809004f, -0.303530f, 0.187599f, 0.934171f, 0.000000f, 0.000000f},
	{-0.587726f, 0.000000f, 0.809010f, -0.303530f, 0.187599f, 0.934171f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, 0.525713f, -0.534589f, 0.330396f, 0.777851f, 0.000000f, 0.000000f},
	{-0.587726f, 0.000000f, 0.809010f, -0.534589f, 0.330396f, 0.777851f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, 0.809004f, -0.534589f, 0.330396f, 0.777851f, 0.000000f, 0.000000f},
	{-0.587726f, 0.000000f, 0.809010f, -0.306567f, -0.125652f, 0.943519f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, 0.850633f, -0.306567f, -0.125652f, 0.943519f, 0.000000f, 0.000000f},
	{0.000060f, 0.000000f, 0.999993f, -0.306567f, -0.125652f, 0.943519f, 0.000000f, 0.000000f},
	{-0.850588f, 0.525736f, -0.000007f, -0.904980f, 0.330393f, 0.268049f, 0.000000f, 0.000000f},
	{-0.950998f, 0.000000f, 0.309006f, -0.904980f, 0.330393f, 0.268049f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, 0.525713f, -0.904980f, 0.330393f, 0.268049f, 0.000000f, 0.000000f},
	{-0.950998f, 0.000000f, 0.309006f, -0.982246f, 0.187599f, 0.000000f, 0.000000f, 0.000000f},
	{-0.850588f, 0.525736f, -0.000007f, -0.982246f, 0.187599f, 0.000000f, 0.000000f, 0.000000f},
	{-0.950998f, -0.000000f, -0.309020f, -0.982246f, 0.187599f, 0.000000f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, -0.525727f, -0.904981f, 0.330394f, -0.268049f, 0.000000f, 0.000000f},
	{-0.950998f, -0.000000f, -0.309020f, -0.904981f, 0.330394f, -0.268049f, 0.000000f, 0.000000f},
	{-0.850588f, 0.525736f, -0.000007f, -0.904981f, 0.330394f, -0.268049f, 0.000000f, 0.000000f},
	{-0.950998f, -0.000000f, -0.309020f, -0.992077f, -0.125631f, 0.000000f, 0.000000f, 0.000000f},
	{-0.894365f, -0.447215f, -0.000007f, -0.992077f, -0.125631f, 0.000000f, 0.000000f, 0.000000f},
	{-0.950998f, 0.000000f, 0.309006f, -0.992077f, -0.125631f, 0.000000f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, -0.809019f, -0.534591f, 0.330396f, -0.777851f, 0.000000f, 0.000000f},
	{-0.587726f, -0.000000f, -0.809024f, -0.534591f, 0.330396f, -0.777851f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, -0.525727f, -0.534591f, 0.330396f, -0.777851f, 0.000000f, 0.000000f},
	{-0.587726f, -0.000000f, -0.809024f, -0.303530f, 0.187597f, -0.934172f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, -0.809019f, -0.303530f, 0.187597f, -0.934172f, 0.000000f, 0.000000f},
	{0.000060f, -0.000000f, -1.000007f, -0.303530f, 0.187597f, -0.934172f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, -0.850647f, -0.024725f, 0.330395f, -0.943519f, 0.000000f, 0.000000f},
	{0.000060f, -0.000000f, -1.000007f, -0.024725f, 0.330395f, -0.943519f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, -0.809019f, -0.024725f, 0.330395f, -0.943519f, 0.000000f, 0.000000f},
	{0.000060f, -0.000000f, -1.000007f, -0.306568f, -0.125652f, -0.943519f, 0.000000f, 0.000000f},
	{-0.276325f, -0.447215f, -0.850647f, -0.306568f, -0.125652f, -0.943519f, 0.000000f, 0.000000f},
	{-0.587726f, -0.000000f, -0.809024f, -0.306568f, -0.125652f, -0.943519f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, -0.500004f, 0.574584f, 0.330397f, -0.748793f, 0.000000f, 0.000000f},
	{0.587845f, -0.000000f, -0.809024f, 0.574584f, 0.330397f, -0.748793f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, -0.850647f, 0.574584f, 0.330397f, -0.748793f, 0.000000f, 0.000000f},
	{0.587845f, -0.000000f, -0.809024f, 0.794655f, 0.187596f, -0.577349f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, -0.500004f, 0.794655f, 0.187596f, -0.577349f, 0.000000f, 0.000000f},
	{0.951118f, -0.000000f, -0.309020f, 0.794655f, 0.187596f, -0.577349f, 0.000000f, 0.000000f},
	{0.894485f, 0.447215f, -0.000007f, 0.889697f, 0.330387f, -0.315093f, 0.000000f, 0.000000f},
	{0.951118f, -0.000000f, -0.309020f, 0.889697f, 0.330387f, -0.315093f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, -0.500004f, 0.889697f, 0.330387f, -0.315093f, 0.000000f, 0.000000f},
	{0.951118f, -0.000000f, -0.309020f, 0.802606f, -0.125648f, -0.583126f, 0.000000f, 0.000000f},
	{0.723660f, -0.447215f, -0.525727f, 0.802606f, -0.125648f, -0.583126f, 0.000000f, 0.000000f},
	{0.587845f, -0.000000f, -0.809024f, 0.802606f, -0.125648f, -0.583126f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, 0.499988f, 0.408938f, 0.661687f, 0.628442f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, 0.850633f, 0.408938f, 0.661687f, 0.628442f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, 0.499990f, 0.408938f, 0.661687f, 0.628442f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, 0.499990f, 0.491119f, 0.794657f, 0.356823f, 0.000000f, 0.000000f},
	{0.525790f, 0.850652f, -0.000007f, 0.491119f, 0.794657f, 0.356823f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, 0.499988f, 0.491119f, 0.794657f, 0.356823f, 0.000000f, 0.000000f},
	{0.000060f, 1.000000f, -0.000007f, 0.268034f, 0.943523f, 0.194738f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, 0.499988f, 0.268034f, 0.943523f, 0.194738f, 0.000000f, 0.000000f},
	{0.525790f, 0.850652f, -0.000007f, 0.268034f, 0.943523f, 0.194738f, 0.000000f, 0.000000f},
	{0.525790f, 0.850652f, -0.000007f, 0.724044f, 0.661693f, 0.194736f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, 0.499990f, 0.724044f, 0.661693f, 0.194736f, 0.000000f, 0.000000f},
	{0.894485f, 0.447215f, -0.000007f, 0.724044f, 0.661693f, 0.194736f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, 0.309004f, -0.471317f, 0.661688f, 0.583120f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, 0.525713f, -0.471317f, 0.661688f, 0.583120f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, 0.809004f, -0.471317f, 0.661688f, 0.583120f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, 0.809004f, -0.187594f, 0.794658f, 0.577345f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, 0.499988f, -0.187594f, 0.794658f, 0.577345f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, 0.309004f, -0.187594f, 0.794658f, 0.577345f, 0.000000f, 0.000000f},
	{0.000060f, 1.000000f, -0.000007f, -0.102381f, 0.943523f, 0.315091f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, 0.309004f, -0.102381f, 0.943523f, 0.315091f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, 0.499988f, -0.102381f, 0.943523f, 0.315091f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, 0.499988f, 0.038547f, 0.661687f, 0.748788f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, 0.809004f, 0.038547f, 0.661687f, 0.748788f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, 0.850633f, 0.038547f, 0.661687f, 0.748788f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, -0.309019f, -0.700228f, 0.661687f, -0.268049f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, -0.525727f, -0.700228f, 0.661687f, -0.268049f, 0.000000f, 0.000000f},
	{-0.850588f, 0.525736f, -0.000007f, -0.700228f, 0.661687f, -0.268049f, 0.000000f, 0.000000f},
	{-0.850588f, 0.525736f, -0.000007f, -0.607060f, 0.794656f, 0.000000f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, 0.309004f, -0.607060f, 0.794656f, 0.000000f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, -0.309019f, -0.607060f, 0.794656f, 0.000000f, 0.000000f, 0.000000f},
	{0.000060f, 1.000000f, -0.000007f, -0.331305f, 0.943524f, 0.000000f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, -0.309019f, -0.331305f, 0.943524f, 0.000000f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, 0.309004f, -0.331305f, 0.943524f, 0.000000f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, 0.309004f, -0.700228f, 0.661688f, 0.268049f, 0.000000f, 0.000000f},
	{-0.850588f, 0.525736f, -0.000007f, -0.700228f, 0.661688f, 0.268049f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, 0.525713f, -0.700228f, 0.661688f, 0.268049f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, -0.500002f, 0.038548f, 0.661687f, -0.748788f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, -0.850647f, 0.038548f, 0.661687f, -0.748788f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, -0.809019f, 0.038548f, 0.661687f, -0.748788f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, -0.809019f, -0.187593f, 0.794658f, -0.577345f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, -0.309019f, -0.187593f, 0.794658f, -0.577345f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, -0.500002f, -0.187593f, 0.794658f, -0.577345f, 0.000000f, 0.000000f},
	{0.000060f, 1.000000f, -0.000007f, -0.102380f, 0.943523f, -0.315090f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, -0.500002f, -0.102380f, 0.943523f, -0.315090f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, -0.309019f, -0.102380f, 0.943523f, -0.315090f, 0.000000f, 0.000000f},
	{-0.425263f, 0.850654f, -0.309019f, -0.471318f, 0.661687f, -0.583120f, 0.000000f, 0.000000f},
	{-0.262809f, 0.525738f, -0.809019f, -0.471318f, 0.661687f, -0.583120f, 0.000000f, 0.000000f},
	{-0.723540f, 0.447215f, -0.525727f, -0.471318f, 0.661687f, -0.583120f, 0.000000f, 0.000000f},
	{0.525790f, 0.850652f, -0.000007f, 0.724044f, 0.661693f, -0.194736f, 0.000000f, 0.000000f},
	{0.894485f, 0.447215f, -0.000007f, 0.724044f, 0.661693f, -0.194736f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, -0.500004f, 0.724044f, 0.661693f, -0.194736f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, -0.500004f, 0.491119f, 0.794657f, -0.356823f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, -0.500002f, 0.491119f, 0.794657f, -0.356823f, 0.000000f, 0.000000f},
	{0.525790f, 0.850652f, -0.000007f, 0.491119f, 0.794657f, -0.356823f, 0.000000f, 0.000000f},
	{0.000060f, 1.000000f, -0.000007f, 0.268034f, 0.943523f, -0.194738f, 0.000000f, 0.000000f},
	{0.525790f, 0.850652f, -0.000007f, 0.268034f, 0.943523f, -0.194738f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, -0.500002f, 0.268034f, 0.943523f, -0.194738f, 0.000000f, 0.000000f},
	{0.162515f, 0.850654f, -0.500002f, 0.408938f, 0.661687f, -0.628442f, 0.000000f, 0.000000f},
	{0.688249f, 0.525736f, -0.500004f, 0.408938f, 0.661687f, -0.628442f, 0.000000f, 0.000000f},
	{0.276445f, 0.447215f, -0.850647f, 0.408938f, 0.661687f, -0.628442f, 0.000000f, 0.000000f},
};

//#define INX_TYPE GL_UNSIGNED_SHORT
unsigned short Mindexes[]={
/* ICO 80 faces */
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 
};

	
	Mesh* m = new Mesh();
	//Size of the array.
	int n = (sizeof(Mindexes)/sizeof(Mindexes[0]));

	m -> numVertices = vertex_count[0];
	m -> vertices = new Vector3[m->numVertices];
	m -> normals = new Vector3[m->numVertices];
	m -> colours = NULL;
	m->bumpTexture = NULL;
	m->textureCoords = NULL;
	m->indices = NULL;

	//Getting the mesh data from ICO.h
	for( int i = 0; i < n; i++){
		m -> vertices [i] = Vector3 ((float)Mvertices[i].x, (float)Mvertices[i].y, (float)Mvertices[i].z);
		m -> normals [i] = Vector3 ((float)Mvertices[i].nx, (float)Mvertices[i].ny, (float)Mvertices[i].nz);
	}


	m -> BufferData();
	return m;
	
}


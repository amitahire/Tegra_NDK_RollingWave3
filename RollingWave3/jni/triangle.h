#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#pragma once

#include <GLES2/gl2.h>
#include "Common.h"
#include "Mesh.h"
#include "Camera.h"
#include "HeightMap.h"
#include "../Team Project/Team Project/Renderer/SceneNode.h"
#include "../Team Project/Team Project/Physics/PhysicsSystem.h"
#include "../Team Project/Team Project/Games/GameEntity.h"
#include	"../nclgl/Light.h"

#include	"engine.h"

#include	"stdio.h"
#include	"stdlib.h"

////Nvidia - Utilities.
#include	"nv_math/nv_matrix.h"
#include	"nv_math/nv_quat.h"
#include	"nv_shader/nv_shader.h"
#include	"nv_glesutil/nv_images.h"

//Vector classes.
#include	"nv_debug.h"


/**
 * Class that holds all of the application's state data. Its methods
 * need to be called from a driving application loop, such as NativeActivity
 */

//For debug-logging.
#define		APP_NAME			"RollingWave3/triangle.h"

//Defining shaders.
#define		SIMPLE_SHADER		"simple"
#define		TEX_SHADER			"simpleTEX"
#define		HEIGHTMAP_SHADER	"heightMap"
#define		SCENENODE_SHADER	"sceneNode"
#define		DEBUG_SHADER		"Debug"
#define		LIGHT_SHADER		"light"
#define		MAINBALL_SHADER		"mainball"
#define		QUAD_SHADER			"quad"
#define		BALL_SHADER			"ball"

class		Shader;
class		SceneNode;
class		GameEntity;

#define		RENDER_HZ	60
#define		PHYSICS_HZ	240
#define		PHYSICS_TIMESTEP (1000.0f / (float)PHYSICS_HZ)


class triangle  {

protected:
	
	static	triangle* appInstance;
	static	PhysicsSystem* physicsInstance;
	GLuint	shaderProg;
	GLuint	shaderProgTEX;
	Mesh*	tri;
	Mesh*	quad;

	HeightMap* heightMap;

	//SceneNodes.
	SceneNode* root;

	//Light
	Light	*light;

	//Shaders
	GLuint	simple_shader;
	GLuint	shaderTEX;
	GLuint	shaderHeightMap;
	GLuint	shaderSceneNode;
	GLuint	shaderDebug;
	GLuint	shaderLight;
	GLuint	shaderMainBall;
	GLuint	shaderQuad;
	GLuint	shaderBall;

	float	renderCounter;
	float	physicsCounter;
	float	renderTime;


	bool	m_bInitialized;
	int32_t m_iWinWidth, m_iWinHeight;
	bool	m_bPauseAnim;

	//For Camera.cpp - (O - Rendere.h)
	float	scale;
	float	rotation;
	float	fov;
	Vector3 position;

	unsigned int width;
	unsigned int height;

	//Protected methods
	bool	resetValues(void);
	bool	initTexturesGL(void);
	bool	initShadersGL(void);
	bool	initMeshesGL(void);
	bool	initLight(void);
	bool	initStateGL(void);
	bool	resizeGL(void);
	bool	positionCameraGL(void);
	bool	switchProgramGL(void);
	bool	cleanupGL(void);

	void	SetDefaultAttributes(GLuint shader_name);
	void	DrawIco();
	void	DrawLight();
	void	DrawHeightMap();
	void	DrawQuad();
	void	DrawRootMain();
	Mesh* MeshData();

public:
	
	triangle(void);
	virtual ~triangle(void);

	static	Mesh* ico;

	Camera* camera;

	Matrix4 modelMatrix;
	Matrix4 projMatrix;
	Matrix4 viewMatrix;
	Matrix4 textureMatrix;

	//FPS calculation.
	float	FPS;
	float	GetFPS(){ return FPS;}

	//Public methods.
	bool	init();
	bool	resize(const int width, const int height);
	bool	render(const float current_time);
	bool	positionCamera(void);
	bool	cleanup(void);
	bool	restart();

	//Update function.
	void	UpdateCore(float msec); // msec == deltatime. 
	void	UpdateScene(float msec);
	void	UpdateGame(float msec);
	bool	renderGL(float current_time = NULL);

	//Scene Management
	void	DrawRoot();
	void	AddNode(SceneNode* n);
	void	RemoveNode(SceneNode* n);

	//SceneNodes - Draw stuff.
	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
	void	DrawNode(SceneNode* n);

	bool toggleCamera;

	//Vectors
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	vector<SceneNode*> lightList;
	std::vector<GameEntity*> allEntities;


	//GameEntity parts.
	GameEntity* BuildMainBall(Vector3 pos);
	GameEntity* BuildBall(Vector3 pos);
	GameEntity* BuildQuadPlane(Vector3 pos);

	//Static Class init.
	static triangle &GetTriangle() { return *appInstance;}
	static PhysicsSystem &GetPhysicsSystem() {return *physicsInstance;}

	// For Camera.cpp (O - Rendere.h)
	void SwitchToPerspective();
	void SwitchToOrthographic(float fov = 45.0f);

	inline void SetScale(float s) {scale =s;}
	inline void SetRotation(float r) {rotation =r;}
	inline void SetPosition(Vector3 p) {position = p;}
	inline void SetFov(float fov) { fov = fov;}
	//virtual void UpdateScene(float msec);

	inline unsigned int getWinWidth(void)  const {return width;}
	inline unsigned int getWinHeight(void) const {return height;}
	inline void setWinWidth(unsigned int w) { width = w; }
	inline void setWinHeight(unsigned int h) { height = h; }
	inline void pauseAnim(bool doPause)   {m_bPauseAnim = doPause;}
	inline bool isInitialized() { return m_bInitialized; }

};

#endif /* TRIANGLE_H_ */



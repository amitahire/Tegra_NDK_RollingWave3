#pragma once
#include "../Team Project/Team Project/Games/GameEntity.h"

class MainBall : public GameEntity
{

public:
	MainBall(void);

	MainBall(SceneNode* s, PhysicsNode* p);
	~MainBall(void);

	//bool MainBall;

	virtual void Update(float msec);

	PhysicsSystem* psys;



};


#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;

class Player : public Entity
{
public:

	EntityState StateMachine() override;
	void Move() override;
	void Jump() override;
	void Climb() override;

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	// Set animations
	void setIdleAnimation();
	void setMoveAnimation();
	void setJumpAnimation();
	void setClimbAnimation();
	void setWinAnimation();

public:
	int isAlive = true;

	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* groundSensor;
	PhysBody* topSensor;
	PhysBody* leftSensor;
	PhysBody* rightSensor;
	int pickCoinFxId;

	//Movement
	bool inAir = false;
	int direction = 0;
	float moveForce = 1.0f;
	bool isGrounded = false;
	bool isCollidingTop = false;
	bool isCollidingLeft = false;
	bool isCollidingRight = false;
};

#endif // __PLAYER_H__
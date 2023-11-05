#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Animation.h"
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

	void debugTools();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void EndCollision(PhysBody* physA, PhysBody* physB);

	// Set animations
	void setIdleAnimation();
	void setMoveAnimation();
	void setJumpAnimation();
	void setClimbAnimation();
	void setWinAnimation();

	void CopyParentRotation(PhysBody* parent, PhysBody* child, float xOffset, float yOffset, float angleOffset);

public:
	//Animations
	Animation idleAnim;
	Animation walkAnim;
	Animation jumpAnim;
	Animation* currentAnimation = nullptr;

	int isAlive = true;

	// Check if the player has won
	int win = false;

	// debug attributes
	bool godMode = false;
	bool noClip = false;
	
	float maxSpeed = 2.0f;
	const char* texturePath;
	iPoint spawnPosition;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* groundSensor;
	PhysBody* topSensor;
	PhysBody* leftSensor;
	PhysBody* rightSensor;
	int pickCoinFxId;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Movement
	bool inAir = false;
	int direction = 0;
	int angle = 0;
	float moveForce = 1.0f;
	bool isGrounded = false;
	bool isCollidingTop = false;
	bool isCollidingLeft = false;
	bool isCollidingRight = false;
};

#endif // __PLAYER_H__
#ifndef __OWLENEMY_H__
#define __OWLENEMY_H__

#include "Animation.h"
#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;

class OwlEnemy : public Entity
{
public:

	bool startTimer = true;
	Timer timer;
	Timer movementDelay;

	EntityState StateMachine() override;
	void Move() override;
	void Jump() override;

	bool SaveState(pugi::xml_node& node) override;
	bool LoadState(pugi::xml_node& node) override;

	OwlEnemy();

	virtual ~OwlEnemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void EndCollision(PhysBody* physA, PhysBody* physB);

	// Set animations
	void setIdleAnimation();
	void setMoveAnimation();
	void setJumpAnimation();

	void CopyParentRotation(PhysBody* parent, PhysBody* child, float xOffset, float yOffset, float angleOffset);

	void moveToSpawnPoint();

public:
	//Animations
	Animation idleAnim;
	Animation walkAnim;
	Animation jumpAnim;
	Animation* currentAnimation = nullptr;

	int isAlive = true;

	float maxSpeed = 2.0f;
	const char* texturePath;
	iPoint spawnPosition;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Movement
	bool inAir = false;
	int direction = 0;
	int angle = 0;
	float moveForce = 1.0f;
	bool isGrounded = false;
	int moveSpeed = 2;

	int currentPathPos;

};

#endif // __OWLENEMY_H__
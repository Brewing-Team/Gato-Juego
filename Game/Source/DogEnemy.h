#ifndef __DOGENEMY_H__
#define __DOGENEMY_H__

#include "Animation.h"
#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#include <Box2D/Box2D.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#include "Box2D/Box2D/Box2D.h"
#endif

struct SDL_Texture;

class DogEnemy : public Entity
{
public:

	bool startTimer = true;
	Timer timer;
	Timer movementDelay;

	EntityState StateMachine(float dt) override;
	void Move(float dt) override;
	void Jump(float dt) override;

	bool SaveState(pugi::xml_node& node) override;
	bool LoadState(pugi::xml_node& node) override;

	DogEnemy();

	virtual ~DogEnemy();

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

	void moveToSpawnPoint();

public:
	//Animations
	Animation idleAnim;
	Animation flyAnim;
	Animation jumpAnim;
	Animation* currentAnimation = nullptr;

	int isAlive = true;

	float maxSpeed = 2.0f;
	const char* texturePath;
	iPoint spawnPosition;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* groundSensor;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Movement
	int moveSpeed = 2;
	int angle;
	b2Vec2 movementDirection;
	bool isGrounded = false;

	int currentPathPos;

};

#endif // __DOGENEMY_H__
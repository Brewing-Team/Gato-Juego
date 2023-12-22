#ifndef __DOGENEMY_H__
#define __DOGENEMY_H__

#include "Animation.h"
#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
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

	bool Awake() override;

	bool Start() override;

	bool Update(float dt) override;

	bool CleanUp() override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void EndCollision(PhysBody* physA, PhysBody* physB) override;

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

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Movement
	int moveSpeed = 2;
	int angle;
	iPoint movementDirection;

	int currentPathPos;

};

#endif // __DOGENEMY_H__
#ifndef __OWLENEMY_H__
#define __OWLENEMY_H__

#include "Animation.h"
#include "Entity.h"
#include "Point.h"
#include "Player.h"
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

	EntityState StateMachine(float dt) override;
	void Idle(float dt);
	void Move(float dt) override;
	void Attack(float dt) override;

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

	void moveToSpawnPoint();
	void pathfindingMovement(float dt);

public:
	//Animations
	Animation idleAnim;
	Animation flyAnim;
	Animation hurtedAnim;
	Animation sleepingAnim;
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

	int currentPathPos;
	
	//Attack
	Timer attackTimer;

private:

	Player* player;
	int lives = 3;
	bool invencible = false;

};

#endif // __OWLENEMY_H__
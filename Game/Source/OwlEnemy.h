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

	//EntityState StateMachine(float dt) override;
	void Idle(float dt);
	void Move(float dt) override;
	void Attack(float dt) override;

	bool SaveState(pugi::xml_node& node) override;
	bool LoadState(pugi::xml_node& node) override;

	OwlEnemy();

	virtual ~OwlEnemy();

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
	void pathfindingMovement(float dt);

public:
	//Animations
	Animation idleAnim;
	Animation flyAnim;
	Animation hurtedAnim;
	Animation sleepingAnim;
	Animation jumpAnim;
	Animation* currentAnimation = nullptr;

	//audios
	int owlDeath;
	int owlHit;
	int owlAttack;
	int owlIdle;

	int isAlive = true;

	float maxSpeed = 2.0f;
	const char* texturePath;
	iPoint spawnPosition;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;

	StateMachine<OwlEnemy>* movementStateMachine;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Movement
	int moveSpeed = 2;
	int angle;

	int currentPathPos;
	
	//Attack
	Timer attackTimer;

	bool invencible = false;
	Timer reviveTimer;
	int lives = 3;

private:

	Player* player;

};

#endif // __OWLENEMY_H__
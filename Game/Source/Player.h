#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Animation.h"
#include "Entity.h"
#include "FurBall.h"
#include "Physics.h"
#include "Point.h"
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Fixture.h>
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;

class Player : public Entity
{
public:

	bool startTimer = true;
	Timer timer;

	EntityState StateMachine(float dt) override;
	void Move(float dt) override;
	void Jump(float dt) override;
	void Climb(float dt) override;

	bool SaveState(pugi::xml_node& node) override;
	bool LoadState(pugi::xml_node& node) override;

	Player();
	
	virtual ~Player();

	bool Awake() override;

	bool Start() override;

	bool Update(float dt) override;

	void debugTools();

	bool CleanUp() override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void EndCollision(PhysBody* physA, PhysBody* physB) override;

	void OnRaycastHit(b2Fixture* fixture, const b2Vec2& point,
                       const b2Vec2& normal, float32 fraction) override;

	// Set animations
	void setIdleAnimation();
	void setMoveAnimation();
	void setJumpAnimation();
	void setClimbAnimation();
	void setWinAnimation();

	void CopyParentRotation(PhysBody* parent, PhysBody* child, float xOffset, float yOffset, float angleOffset);

	void moveToSpawnPoint();

public:
	//Animations
	Animation idleAnim;
	Animation walkAnim;
	Animation jumpAnim;
	Animation* currentAnimation = nullptr;

	int isAlive = true;

	int lives = 7;
	Timer immunityTimer;

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

	Raycast* raycastTest;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Movement
	bool inAir = false;
	int direction = 0;
	int angle = 0;
	float moveForce = 1.0f;
	bool isGrounded = false;
	bool isCollidingTop = false;
	bool isCollidingLeft = false;
	bool climbingLeft = false;
	bool isCollidingRight = false;
	bool climbingRight = false;

	float bulletSpeed = 2.0f;

	// debug textures
	SDL_Texture* debugMenuTexture;

	//REMOVE
	b2Vec2 pointTest;
	b2Vec2 normalTest;
};

#endif // __PLAYER_H__
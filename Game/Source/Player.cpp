#include "Player.h"
#include "App.h"
#include "Entity.h"
#include "Map.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "FurBall.h"

#include "Window.h"
#include <cmath>
#include <iostream>

#ifdef __linux__
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#endif

void Player::setIdleAnimation()
{
	currentAnimation = &idleAnim;
}

void Player::setMoveAnimation()
{
	currentAnimation = &walkAnim;
	jumpAnim.Reset();
}

void Player::setJumpAnimation()
{
	currentAnimation = &jumpAnim;
}

void Player::setClimbAnimation()
{
	currentAnimation = &walkAnim;
}

void Player::setWinAnimation()
{
	// TODO set win animation
}

void Player::Move(float dt) {
	
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		if (pbody->body->GetLinearVelocity().x >= -maxSpeed)
		{
			float impulse = pbody->body->GetMass() * moveForce;
			pbody->body->ApplyLinearImpulse({ -impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
		flip = SDL_FLIP_HORIZONTAL;
	}

	else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		if(pbody->body->GetLinearVelocity().x <= maxSpeed)
		{
			float impulse = pbody->body->GetMass() * moveForce;
			pbody->body->ApplyLinearImpulse({ impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
		flip = SDL_FLIP_NONE;
	}
	else if(isGrounded){
		state = EntityState::IDLE;
	}

}

void Player::Jump(float dt) {
	
	float impulse = pbody->body->GetMass() * 5;
	pbody->body->ApplyLinearImpulse(b2Vec2(0, -impulse), pbody->body->GetWorldCenter(), true);
	isGrounded = false;
	
}

void Player::Climb(float dt) {
	
	if (startTimer) {
		timer.Start();
		startTimer = false;
	}

	LOG("TIMER: %d", timer.ReadSec());

	if (timer.ReadSec() <= 5) {
		if (isCollidingRight) {
			climbingLeft = false;
			climbingRight = true;
			flip = SDL_FLIP_NONE;
		}

		else if (isCollidingLeft) {
			climbingRight = false;
			climbingLeft = true;
			flip = SDL_FLIP_HORIZONTAL;
		}

		if (climbingRight) {
			angle = std::lerp(angle, -90, dt * 32 / 1000);
			pbody->body->ApplyForceToCenter({ 1, 0 }, true);
		}
		else if (climbingLeft) {
			angle = std::lerp(angle, 90, dt * 32 / 1000);
			pbody->body->ApplyForceToCenter({ -1, 0 }, true);
		}

		pbody->body->ApplyForceToCenter({ 0, -2.0f }, true);

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {

			if (pbody->body->GetLinearVelocity().y >= -maxSpeed)
			{
				float impulse = pbody->body->GetMass() * 1;
				pbody->body->ApplyLinearImpulse({ 0, -impulse }, pbody->body->GetWorldCenter(), true);
			}

		}

		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {

			if (pbody->body->GetLinearVelocity().y >= -maxSpeed)
			{
				float impulse = pbody->body->GetMass() * 1;
				pbody->body->ApplyLinearImpulse({ 0, impulse }, pbody->body->GetWorldCenter(), true);
			}

		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

			float impulse = pbody->body->GetMass() * 5;
			pbody->body->ApplyLinearImpulse({ impulse * (float32)SDL_sin(DEGTORAD*angle), 0 }, pbody->body->GetWorldCenter(), true);

			flip = (flip == SDL_FLIP_HORIZONTAL) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

		}
	
	} else {
		
		startTimer = true;
		state = EntityState::IDLE;

	}

}

bool Player::SaveState(pugi::xml_node& node) {

	pugi::xml_node playerAttributes = node.append_child("player");
	playerAttributes.append_attribute("x").set_value(this->position.x);
	playerAttributes.append_attribute("y").set_value(this->position.y);
	playerAttributes.append_attribute("angle").set_value(this->angle);

	return true;

}

bool Player::LoadState(pugi::xml_node& node)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(node.child("player").attribute("x").as_int()), PIXEL_TO_METERS(node.child("player").attribute("y").as_int()) }, node.child("player").attribute("angle").as_int());

	// reset player physics
	pbody->body->SetAwake(false);
	pbody->body->SetAwake(true);

	return true;
}

EntityState Player::StateMachine(float dt) {

	switch (this->state) {

		case EntityState::IDLE:

			angle = std::lerp(angle, 0, dt * 32 / 1000);;

			setIdleAnimation();

			if (!isAlive and !godMode) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			if (isGrounded) {
				if(app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN){
					Jump(dt);
					jumpAnim.Reset();
				}
			}
			else
			{
				setJumpAnimation();
			}

			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT or app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				this->state = EntityState::MOVE;
			}

			break;
		case EntityState::MOVE:

			angle = std::lerp(angle, 0, dt * 32 / 1000);

			if(isGrounded)
			{
				setMoveAnimation();
			}
			else
			{
				setJumpAnimation();
			}

			if (!isAlive and !godMode) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			if (isGrounded && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

				Jump(dt);
				Move(dt);

			} else {

				Move(dt);

			}

			break;

		case EntityState::CLIMB:

			if(isGrounded)
			{
				if ((int)pbody->body->GetLinearVelocity().y == 0) //esto rarete pero buenop
				{
					setIdleAnimation();
				}
				else{
					setClimbAnimation();
				}
			}
			else
			{
				setJumpAnimation();
			}
			

			if (!isAlive and !godMode) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			if (!isGrounded and !isCollidingLeft and !isCollidingRight) {
				this->state = EntityState::IDLE;
			}
			
			Climb(dt);
			


			break;

		case EntityState::HURT:

			currentAnimation = &hurtAnim;
			if (currentAnimation->HasFinished()){
					hurtAnim.Reset();
					hurtAnim.ResetLoopCount();
					state = EntityState::IDLE;
			}

			break;

		case EntityState::WIN:

			// TODO hacer cosa de ganar jugador ole ole

			moveToSpawnPoint();

			state = EntityState::IDLE;

			break;

		case EntityState::DEAD:

			setWinAnimation();

			// TODO resetear mundo, restar vidas, etc

			moveToSpawnPoint();

			isAlive = true;
			lives = 7;
			state = EntityState::IDLE;

			break;

		case EntityState::NO_CLIP:

			setIdleAnimation();

			if (noClip) {
				// deactivate physics
				this->pbody->body->SetAwake(false);

				// Deactivate gravity
				pbody->body->SetGravityScale(0);

				// debug controls
				if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
					pbody->body->SetLinearVelocity({ -3,0});
				}

				if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
					pbody->body->SetLinearVelocity({ 3,0});
				}

				if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
					pbody->body->SetLinearVelocity({ 0,-3});
				}

				if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
					pbody->body->SetLinearVelocity({ 0,3 });
				}
			} else {
				// activate physics
				this->pbody->body->GetFixtureList()->SetSensor(false);
				this->pbody->body->SetAwake(true);

				// Activate Gravity
				pbody->body->SetGravityScale(1);

				state = EntityState::IDLE;
			}

			break;

	}

	return this->state;

}

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
	state = EntityState::IDLE;
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	spawnPosition = position;
	
	return true;
}

bool Player::Start() {

	timer = Timer();

	//load Animations TODO: identify animations by name (en teoria ya esta hecho pero hay que hacer la funcion que te devuelve la animacion por nombre)
	walkAnim = *app->map->GetAnimByName("Cat-1-Walk");
	walkAnim.speed = 8.0f;
	idleAnim = *app->map->GetAnimByName("Cat-1-Idle");
	idleAnim.speed = 8.0f;
	jumpAnim = *app->map->GetAnimByName("Cat-1-Run");
	jumpAnim.speed = 8.0f;
	hurtAnim = *app->map->GetAnimByName("Cat-1-Hurt");
	hurtAnim.speed = 16.0f;
	hurtAnim.loop = false;

	currentAnimation = &idleAnim;

	//pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody = app->physics->CreateRectangle(position.x, position.y, 20, 10, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	//si quieres dar vueltos como la helice de un helicoptero Boeing AH-64 Apache pon en false la siguiente funcion
	pbody->body->SetFixedRotation(true);
	pbody->body->GetFixtureList()->SetFriction(25.0f);
	pbody->body->SetLinearDamping(1);

	// Create player sensors
	groundSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 10, 5, bodyType::DYNAMIC);
	groundSensor->listener = this;

	topSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 10, 1, bodyType::DYNAMIC);
	topSensor->listener = this;

	leftSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 1, 5, bodyType::DYNAMIC);
	leftSensor->listener = this;

	rightSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 1, 5, bodyType::DYNAMIC);
	rightSensor->listener = this;
	
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	// TODO load debug menu texture from xml
	// load debug menu texture
	debugMenuTexture = app->tex->Load("Assets/Textures/debug_menu.png");


	// TODO test ropes
	/*
	app->physics->CreateRope(20, {21,28}, {25, 32});
	//app->physics->CreateRope(20, {5,0}, {10, 0});
	app->physics->CreateRope(20);
	app->physics->CreateRope(20, {0,0});
	*/

	return true;
}

bool Player::Update(float dt)
{
	LOG("%d", lives);
	debugTools();

 	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (shootCooldown.ReadMSec() > shootCooldownTime)
		{
			b2Vec2 mouseWorldPosition = { PIXEL_TO_METERS(app->input->GetMouseX()) + PIXEL_TO_METERS(-app->render->camera.x), PIXEL_TO_METERS(app->input->GetMouseY()) + PIXEL_TO_METERS(-app->render->camera.y) };
			b2Vec2 shootDir = {mouseWorldPosition - pbody->body->GetPosition()};
			shootDir.Normalize();

			FurBall* bullet = (FurBall*)app->entityManager->CreateEntity(EntityType::FURBALL);
			bullet->Awake();
			bullet->Start();

			bullet->pbody->body->SetTransform(pbody->body->GetPosition() + b2Vec2{0, 0}, 0);
			bullet->pbody->body->SetAwake(false);
			bullet->pbody->body->ApplyForce({ shootDir.x * bulletSpeed, shootDir.y * bulletSpeed}, bullet->pbody->body->GetWorldCenter(), true);
			shootCooldown.Start();
		}
	}
	//debug shootDir
	if(debug)
		{
			b2Vec2 mouseWorldPosition = { PIXEL_TO_METERS(app->input->GetMouseX()) + PIXEL_TO_METERS(-app->render->camera.x), PIXEL_TO_METERS(app->input->GetMouseY()) + PIXEL_TO_METERS(-app->render->camera.y) };
			app->render->DrawLine(METERS_TO_PIXELS(pbody->body->GetPosition().x), METERS_TO_PIXELS(pbody->body->GetPosition().y), METERS_TO_PIXELS(mouseWorldPosition.x), METERS_TO_PIXELS(mouseWorldPosition.y), 255, 0, 0);
		}

	// Update player state

	if(state == EntityState::MOVE) {
		if (isCollidingLeft or isCollidingRight and !isGrounded) {
			state = EntityState::CLIMB;
		}
	}

	StateMachine(dt);
	//LOG("state: %d", state);

	pbody->body->SetTransform(pbody->body->GetPosition(), angle*DEGTORAD);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	// Update player sensors
	CopyParentRotation(pbody, groundSensor, -12, -2, 270);

	CopyParentRotation(pbody, topSensor, -14, 0, 90);

	CopyParentRotation(pbody, leftSensor, 0, 1, 0);

	CopyParentRotation(pbody, rightSensor, 0, 1, 180);	
	
	//SDL_Rect rect = { 0,0,50,50 };
	app->render->DrawTexture(currentAnimation->texture, position.x - 9, position.y - 9, &currentAnimation->GetCurrentFrame(), 1.0f, pbody->body->GetAngle()*RADTODEG, flip);

	currentAnimation->Update(dt);
	return true;
}

void Player::debugTools()
{
	// DEBUG TOOLS ------------------------------------------------

	if (debug) {
		// Draw debug menu

		//app->render->DrawTexture(debugMenuTexture, position.x, position.y);
		//app->render->DrawTexture(debugMenuTexture, app->render->camera.x, app->render->camera.y);
	}

	// Toggle on/off debug mode + View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
		
	}

	if (debug) {
		// Teleport Menu Level
		if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
			// Resetart Level 1
			moveToSpawnPoint();
			// Resetart Level 2
			// TODO teleport player to the spawnPosition of the next level
			// position = spawnPosition;
		}

		// Restart current level (Kill player)
		if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
			lives = 0;
			isAlive = false;
		}

		// Toggle free cam mode on/off
		if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
			freeCam = !freeCam;
		}
		// Save game state
		if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();

		// Load game state
		if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();

		// Toggle god mode on/off
		if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {
			godMode = !godMode;
		}

		// Toggle no-clip mode on/off
		if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
			noClip = !noClip;
			state = EntityState::NO_CLIP;
			this->pbody->body->GetFixtureList()->SetSensor(true);
		}

		// Toggle FPS cap on/off
		if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
			fpsLimiter = !fpsLimiter;
		}
	}

	// END OF DEBUG TOOLS -----------------------------------------
}

void Player::CopyParentRotation(PhysBody* parent, PhysBody* child, float xOffset, float yOffset, float angleOffset)
{
	
	float angle = parent->body->GetAngle();

	child->body->SetTransform(
		b2Vec2(
			parent->body->GetTransform().p.x -
			PIXEL_TO_METERS(SDL_cos(angle + DEGTORAD * angleOffset)) * (parent->width + child->width + xOffset),
			parent->body->GetTransform().p.y - 
			PIXEL_TO_METERS(SDL_sin(angle + DEGTORAD * angleOffset)) * (parent->height + child->height + yOffset)),
			DEGTORAD * parent->GetRotation());
}

void Player::moveToSpawnPoint()
{
	position = spawnPosition;

	pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0);

	// reset player physics
	pbody->body->SetAwake(false);
	pbody->body->SetAwake(true);
}

bool Player::CleanUp() {

	app->tex->UnLoad(debugMenuTexture);
	app->tex->UnLoad(texture);


	// Theres no need to unload audio fx because they are unloaded when te audio module is cleaned up
	// app->audio->UnloadFx(pickCoinFxId);

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	if (physA->body->GetFixtureList()->IsSensor()) {
		if (physB->ctype == ColliderType::PLATFORM) { //Condicion temporal
			if (physA == groundSensor) {
				LOG("Ground collision");
				isGrounded = true;
			}
			else if (physA == leftSensor) {
				LOG("Left collision");
				isCollidingLeft = true;
			}
			else if (physA == rightSensor) {
				LOG("Right collision");
				isCollidingRight = true;
			}
		}
		else if(physB->ctype == ColliderType::ENEMY or physB->ctype == ColliderType::BULLET){
			if (physA == groundSensor) {
				LOG("Ground collision");
				isGrounded = true;
			}
		}
	}
	switch (physB->ctype) {

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;

	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		if (immunityTimer.ReadSec() >= 1){
			if (lives <= 1)
			{
				state = EntityState::DEAD;
			}
			else{
				lives--;
				state = EntityState::HURT;
				immunityTimer.Start();
			}
		}
		break;

	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::DEATH:
		LOG("Collision DEATH");

		if (!godMode){	
		lives = 0;
		isAlive = false;
		}
		break;

	case ColliderType::LIMITS:
		LOG("Collision LIMITS");
		break;
	case ColliderType::WIN:
		state = EntityState::WIN;
		LOG("Collision WIN");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	}
	
}

void Player::EndCollision(PhysBody* physA, PhysBody* physB){

	if (physA->body->GetFixtureList()->IsSensor()) {
		if (physB->ctype == ColliderType::PLATFORM) {
			if (physA == groundSensor) {
				LOG("Ground collision");
				isGrounded = false;
			}
			else if (physA == leftSensor) {
				LOG("Left collision");
				isCollidingLeft = false;
			}
			else if (physA == rightSensor) {
				LOG("Right collision");
				isCollidingRight = false;
			}
		}
	}
	
}

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

//States
#include "States/Player/IdleState.h"

#include "Window.h"
#include <cmath>
#include <iostream>

#ifdef __linux__
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#endif

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
	shootCooldown = Timer(5);

	stateMachineTest = new StateMachine((Player*)this);
	stateMachineTest->AddState(new IdleState("idle"));

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
	
	// Load audios
	playerAttack = app->audio->LoadFx("Assets/Audio/Fx/CatAttack.wav");
	playerAttack2 = app->audio->LoadFx("Assets/Audio/Fx/CatAttack2.wav");
	playerDeath = app->audio->LoadFx("Assets/Audio/Fx/CatDeath.wav");
	playerHit = app->audio->LoadFx("Assets/Audio/Fx/CatHit.wav");
	playerJump = app->audio->LoadFx("Assets/Audio/Fx/CatJump.wav");
	playerWalk = app->audio->LoadFx("Assets/Audio/Fx/CatWalk.ogg");
	playerMeow = app->audio->LoadFx("Assets/Audio/Fx/CatMeow.wav");
	playerWin = app->audio->LoadFx("Assets/Audio/Fx/Win.ogg");
	pickItem = app->audio->LoadFx("Assets/Audio/Fx/PickItem.wav");

	raycastTest = app->physics->CreateRaycast(this, pbody->body->GetPosition(), {pbody->body->GetPosition().x, pbody->body->GetPosition().y + 0.4f});

	// TODO load debug menu texture from xml
	// load debug menu texture
	debugMenuTexture = app->tex->Load("Assets/Textures/debug_menu.png");

	return true;
}

bool Player::Update(float dt)
{
	//LOG("%d", lives);
	stateMachineTest->Update(dt);

	debugTools();

 	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (shootCooldown.ReadMSec() > shootCooldownTime)
		{

			// AUDIO DONE player attack
			app->audio->PlayFx(playerAttack);
			app->audio->PlayFx(playerAttack2);

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

	//StateMachine(dt);
	//LOG("state: %d", state);

	pbody->body->SetTransform(pbody->body->GetPosition(), angle*DEGTORAD);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	//Update Raycast position
	raycastTest->rayStart = pbody->body->GetPosition();
	float32 rotatedX = pbody->body->GetPosition().x + 0.4f * SDL_cos(pbody->body->GetAngle() + DEGTORAD * 90);
	float32 rotatedY = pbody->body->GetPosition().y + 0.4f * SDL_sin(pbody->body->GetAngle() + DEGTORAD * 90);
	raycastTest->rayEnd = { rotatedX, rotatedY };

	// Update player sensors
	CopyParentRotation(pbody, groundSensor, -12, -2, 270);

	CopyParentRotation(pbody, topSensor, -14, 0, 90);

	CopyParentRotation(pbody, leftSensor, 0, 1, 0);

	CopyParentRotation(pbody, rightSensor, 0, 1, 180);	
	
	//SDL_Rect rect = { 0,0,50,50 };
	app->render->DrawTexture(currentAnimation->texture, position.x - 9, position.y - 9, &currentAnimation->GetCurrentFrame(), 1.0f, pbody->body->GetAngle()*RADTODEG, flip);

	currentAnimation->Update(dt);

	//REMOVE
	app->render->DrawRectangle({METERS_TO_PIXELS(pointTest.x) - 1, METERS_TO_PIXELS(pointTest.y) - 1, 2,2}, 0, 0, 255);
	app->render->DrawLine(METERS_TO_PIXELS(pointTest.x), METERS_TO_PIXELS(pointTest.y), METERS_TO_PIXELS(pointTest.x + (normalTest.x * 10)), METERS_TO_PIXELS(pointTest.y + (normalTest.y * 10)), 255, 255, 0);

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

bool Player::SaveState(pugi::xml_node& node) {

	pugi::xml_node playerAttributes = node.append_child("player");
	playerAttributes.append_attribute("x").set_value(this->position.x);
	playerAttributes.append_attribute("y").set_value(this->position.y);
	playerAttributes.append_attribute("angle").set_value(this->angle);
	playerAttributes.append_attribute("lives").set_value(lives);

	return true;

}

bool Player::LoadState(pugi::xml_node& node)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(node.child("player").attribute("x").as_int()), PIXEL_TO_METERS(node.child("player").attribute("y").as_int()) }, node.child("player").attribute("angle").as_int());
	lives = node.child("player").attribute("lives").as_int();
	// reset player physics
	pbody->body->SetAwake(false);
	pbody->body->SetAwake(true);

	return true;
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

		// AUDIO TODO pick item
		app->audio->PlayFx(pickItem);
		break;

	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		if (!godMode) {
			if (immunityTimer.ReadSec() >= 1){
				if (lives <= 1)
				{
					state = EntityState::DEAD;
				}
				else{
					// AUDIO DONE player hit
					app->audio->PlayFx(playerHit);
					lives--;
					state = EntityState::HURT;
					immunityTimer.Start();
				}
			}
		}
		break;

	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::DEATH:
		LOG("Collision DEATH");

		if (!godMode) {
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

void Player::OnRaycastHit(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction){
	LOG("Raycast hit");
    std::cout << "Point: " << point.x << ", " << point.y << std::endl;
    std::cout << "Normal: " << normal.x << ", " << normal.y << std::endl;
    std::cout << "Fraction: " << fraction << std::endl;

	//REMOVE
	pointTest = point;
	normalTest = normal;

/* 	if (state == EntityState::IDLE){
	
	} */

	float32 dot = b2Dot(normal, { 0,-1 });
	float32 det = b2Cross(normal, { 0,-1 });
	angle = -b2Atan2(det, dot) * RADTODEG;
}

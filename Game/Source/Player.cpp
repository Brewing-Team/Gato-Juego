#include "Player.h"
#include "App.h"
#include "Map.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

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
}

void Player::setJumpAnimation()
{
	currentAnimation = &jumpAnim;
}

void Player::setClimbAnimation()
{
	// TODO set climb animation
}

void Player::setWinAnimation()
{
	// TODO set win animation
}

void Player::Move() {
	
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

void Player::Jump() {
	
	float impulse = pbody->body->GetMass() * 5;
	pbody->body->ApplyLinearImpulse(b2Vec2(0, -impulse), pbody->body->GetWorldCenter(), true);
	isGrounded = false;
	
}

void Player::Climb() {

	if (isCollidingRight) {
		angle = -90;
		flip = SDL_FLIP_NONE;
	}

	if (isCollidingLeft) {
		angle = 90;
		flip = SDL_FLIP_HORIZONTAL;
	}

	if(angle == -90){
		pbody->body->ApplyForceToCenter({ 1, 0 }, true);
	} else {
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
		pbody->body->ApplyLinearImpulse({ -impulse, 0 }, pbody->body->GetWorldCenter(), true);
	}

	
	
}

EntityState Player::StateMachine() {

	switch (this->state) {

		case EntityState::IDLE:

			angle = 0;

			setIdleAnimation();

			if (!isAlive) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			if (isGrounded && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
				Jump();
			}

			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT or app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				this->state = EntityState::MOVE;
			}

			break;
		case EntityState::MOVE:

			angle = 0;

			setJumpAnimation();
			setMoveAnimation();

			if (!isAlive) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			if (isGrounded && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

				Jump();
				Move();

			} else {

				Move();

			}

			break;

		case EntityState::CLIMB:

			setClimbAnimation();

			if (!isAlive) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			Climb();
			
			if (!isGrounded and !isCollidingLeft and !isCollidingRight) {
				this->state = EntityState::IDLE;
			}


			break;

		case EntityState::WIN:

			// TODO hacer cosa de ganar jugador ole ole

			break;

		case EntityState::DEAD:

			setWinAnimation();

			// TODO resetear mundo, restar vidas, etc

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

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	//load Animations TODO: identify animations by name (en teoria ya esta hecho pero hay que hacer la funcion que te devuelve la animacion por nombre)
	walkAnim = *app->map->mapData.animations[0];
	walkAnim.speed = 8.0f;
	idleAnim = *app->map->mapData.animations[1];
	idleAnim.speed = 8.0f;
	jumpAnim = *app->map->mapData.animations[2];
	jumpAnim.speed = 8.0f;

	currentAnimation = &idleAnim;

	//pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody = app->physics->CreateRectangle(position.x, position.y, 25, 15, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	//si quieres dar vueltos como la helice de un helicoptero Boeing AH-64 Apache pon en false la siguiente funcion
	pbody->body->SetFixedRotation(true);
	pbody->body->GetFixtureList()->SetFriction(25.0f);
	pbody->body->SetLinearDamping(1);

	// Create player sensors
	groundSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 15, 5, bodyType::DYNAMIC);
	groundSensor->listener = this;

	topSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 15, 5, bodyType::DYNAMIC);
	topSensor->listener = this;

	leftSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 5, 10, bodyType::DYNAMIC);
	leftSensor->listener = this;

	rightSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 5, 10, bodyType::DYNAMIC);
	rightSensor->listener = this;
	
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{

	// DEBUG TOOLS ------------------------------------------------

	// Resetart Level 1
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
		// TODO debug
	}

	// Resetart Level 2
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		debug = !debug;
		// TODO debug
	}

	// Restart current level
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		debug = !debug;
		// TODO debug
	}

	// Toggle no clip
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		freeCam = !freeCam;
	}
	
	// View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		debug = !debug;
		// TODO debug
	}

	// Activate or deactivate GOD mode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		debug = !debug;
		// TODO debug
	}

	// Activate or deactivate FPS cap
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		fpsLimiter = !fpsLimiter;
	}

	// END OF DEBUG TOOLS -----------------------------------------

	// Update player state

	if(state == EntityState::MOVE) {
		if (isCollidingLeft or isCollidingRight and !isGrounded) {
			state = EntityState::CLIMB;
		}
	}

	StateMachine();
	LOG("state: %d", state);

	pbody->body->SetTransform(pbody->body->GetPosition(), angle*DEGTORAD);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	// Update player sensors
	CopyParentRotation(pbody, groundSensor, -17, -2, 270);

	CopyParentRotation(pbody, topSensor, -17, -2, 90);

	CopyParentRotation(pbody, leftSensor, -2, -2, 0);

	CopyParentRotation(pbody, rightSensor, -2, -2, 180);	
	
	//SDL_Rect rect = { 0,0,50,50 };
	app->render->DrawTexture(currentAnimation->texture, position.x - 9, position.y - 9, &currentAnimation->GetCurrentFrame(), 1.0f, pbody->body->GetAngle()*RADTODEG, flip);

	currentAnimation->Update(dt);

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

bool Player::CleanUp() {
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	if(physA->body->GetFixtureList()->IsSensor()) {
		if (physB->ctype == ColliderType::PLATFORM) {
			if (physA == groundSensor) {
				LOG("Ground collision");
				isGrounded = true;
			} else if (physA == leftSensor) {
				LOG("Left collision");
				isCollidingLeft = true;
			} else if (physA == rightSensor) {
				LOG("Right collision");
				isCollidingRight = true;
			} else {
				isGrounded = false;
				isCollidingLeft = false;
				isCollidingRight = false;
			}
		}

	}
	switch (physB->ctype) {

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;

	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	}
}

#include "Player.h"
#include "App.h"
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
	// TODO set idle animation
}

void Player::setMoveAnimation()
{
	// TODO set move animation
}

void Player::setJumpAnimation()
{
	// TODO set jump animation
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
	if (direction == -1) {
		if (pbody->body->GetLinearVelocity().x >= -5)
		{
			float impulse = pbody->body->GetMass() * moveForce;
			pbody->body->ApplyLinearImpulse({ -impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
	}

	if (direction == 1) {
		if(pbody->body->GetLinearVelocity().x <= 5)
		{
			float impulse = pbody->body->GetMass() * moveForce;
			pbody->body->ApplyLinearImpulse({ impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
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
	}
	if(isCollidingLeft) {
		angle = 90;
	}
	pbody->body->ApplyForceToCenter(b2Vec2(1, 0), true);

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		float impulse = pbody->body->GetMass() * 5;
		pbody->body->ApplyLinearImpulse(b2Vec2(0, -impulse), pbody->body->GetWorldCenter(), true);
	}
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		float impulse = pbody->body->GetMass() * 5;
		pbody->body->ApplyLinearImpulse(b2Vec2(0, impulse), pbody->body->GetWorldCenter(), true);
	}
}

EntityState Player::StateMachine() {

	switch (this->state) {

		case EntityState::IDLE:

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

			break;
		case EntityState::MOVE:

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

	StateMachine();

	if(angle != 0){
		state = EntityState::CLIMB;
	}
	else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		direction = -1;
		state = EntityState::MOVE;
	}
	else if(app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		direction = 1;
		state = EntityState::MOVE;
	}
	else if(state != EntityState::JUMP && state != EntityState::CLIMB){
		state = EntityState::IDLE;
	}

	pbody->body->SetTransform(pbody->body->GetPosition(), angle*DEGTORAD);

	/*
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		if (pbody->body->GetLinearVelocity().x >= -5)
		{
			float impulse = pbody->body->GetMass() * moveForce;
			pbody->body->ApplyLinearImpulse({ -impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
	}
	if(app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		if(pbody->body->GetLinearVelocity().x <= 5)
		{
			float impulse = pbody->body->GetMass() * moveForce;
			pbody->body->ApplyLinearImpulse({ impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
		if(isGrounded)
		{
			float impulse = pbody->body->GetMass() * 5;
			pbody->body->ApplyLinearImpulse(b2Vec2(0, -impulse), pbody->body->GetWorldCenter(), true);

			isGrounded = false;
		}
	}
	*/

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	// Update player sensors
/* 	groundSensor->body->SetTransform(
		b2Vec2(
			pbody->body->GetTransform().p.x -
			PIXEL_TO_METERS(SDL_cos(pbody->body->GetAngle() + DEGTORAD * 270) * 0.65) * (pbody->width + 4),
			pbody->body->GetTransform().p.y - 
			PIXEL_TO_METERS(SDL_sin(pbody->body->GetAngle() + DEGTORAD * 270)) * (pbody->height + 4)),
			DEGTORAD * pbody->GetRotation()); */

	CopyParentRotation(pbody, groundSensor, -17, -2, 270);

/* 	topSensor->body->SetTransform(
		b2Vec2(
			pbody->body->GetTransform().p.x -
			PIXEL_TO_METERS(SDL_cos(pbody->body->GetAngle() + DEGTORAD * 90) * 0.65) * (pbody->width + 4),
			pbody->body->GetTransform().p.y - 
			PIXEL_TO_METERS(SDL_sin(pbody->body->GetAngle() + DEGTORAD * 90)) * (pbody->height + 4)),
			DEGTORAD * pbody->GetRotation()); */

	CopyParentRotation(pbody, topSensor, -17, -2, 90);

/* 	leftSensor->body->SetTransform(
		b2Vec2(
			pbody->body->GetTransform().p.x -
			PIXEL_TO_METERS(SDL_cos(pbody->body->GetAngle() + DEGTORAD) * 0.65) * (pbody->width + 10),
			pbody->body->GetTransform().p.y - 
			PIXEL_TO_METERS(SDL_sin(pbody->body->GetAngle() + DEGTORAD)) * (pbody->height + 8)),
			DEGTORAD * pbody->GetRotation()); */

	CopyParentRotation(pbody, leftSensor, -2, -2, 0);

	/* rightSensor->body->SetTransform(
		b2Vec2(
			pbody->body->GetTransform().p.x -
			PIXEL_TO_METERS(SDL_cos(pbody->body->GetAngle() + DEGTORAD * 180) * 0.65) * (pbody->width + 10),
			pbody->body->GetTransform().p.y - 
			PIXEL_TO_METERS(SDL_sin(pbody->body->GetAngle() + DEGTORAD * 180)) * (pbody->height + 8)),
			DEGTORAD * pbody->GetRotation()); */

	CopyParentRotation(pbody, rightSensor, -2, -2, 180);
		
	//app->physics->CreateWeldJoint(pbody, b2Vec2(pbody->body->GetPosition().x + 1.0f, pbody->body->GetPosition().y), rightSensor, b2Vec2(rightSensor->body->GetPosition().x + 1.0f, rightSensor->body->GetPosition().y + 1.0f), 0, false,false);
	

	//Esto esta aqui temporalmente don't worry :)
	//app->render->camera.x = -position.x + app->render->camera.w / app->win->GetScale() / 2;

	/* Info para el Hugo del futuro:
	
		1. el casteo de int es para que no se rompa al redondear
		2. el "-16" es el offset del tama�o del player
		3. el "4" que multiplica al "dt" es la "followSpeed"
	*/

	//app->render->camera.x = std::ceil(std::lerp(app->render->camera.x, (int)(app->render->camera.w / 2 / app->win->GetScale()) - 16 - position.x, dt * 4 / 1000));
	//app->render->camera.x = std::ceil(std::lerp(app->render->camera.x, -position.x + 200, dt * 4 / 1000)); // esta funciona en escala 4 pero esta hardcodeado

	//app->render->camera.y = std::ceil(std::lerp(app->render->camera.y, (int)(app->render->camera.h / 2 / app->win->GetScale()) - 16 - position.y, dt * 4 / 1000));
	//app->render->camera.y = -position.y + app->render->camera.h / app->win->GetScale() / 2;
	
	SDL_Rect rect = { 0,0,50,50 };
	app->render->DrawTexture(texture, position.x - 9, position.y - 9, &rect, 1.0f, pbody->body->GetAngle()*RADTODEG);

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
    //position.x -= PIXEL_TO_METERS(SDL_cos(angle + DEGTORAD * 90) * xOffset) * (body->GetFixtureList()->GetShape()->m_radius + yOffset);
    //position.y -= PIXEL_TO_METERS(SDL_sin(angle + DEGTORAD * 90) * xOffset) * (body->GetFixtureList()->GetShape()->m_radius + yOffset);
    //angle += DEGTORAD * angleOffset;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	if(physA->body->GetFixtureList()->IsSensor()) {

		if(physB->ctype == ColliderType::PLATFORM)
		{
			if (physA == groundSensor) {
				LOG("Ground colision");
				isGrounded = true;
			}

			if (physA == topSensor) {
				LOG("Top colision");
				isCollidingTop = true;
			}

			if (physA == leftSensor) {
				LOG("Left colision");
				isCollidingLeft = true;
			}

			if (physA == rightSensor) {
				LOG("Right colision");
				isCollidingRight = true;
				state = EntityState::CLIMB;
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

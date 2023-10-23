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

}

void Player::Jump() {
	if(isGrounded) {

		float impulse = pbody->body->GetMass() * 5;
		pbody->body->ApplyLinearImpulse(b2Vec2(0, -impulse), pbody->body->GetWorldCenter(), true);
		isGrounded = false;

	}
	
}

void Player::Climb() {

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

			break;

		case EntityState::MOVE:

			setMoveAnimation();

			if (!isAlive) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			Move();

			break;

		case EntityState::JUMP:

			setJumpAnimation();

			if (!isAlive) {
				this->state = EntityState::DEAD;
			}

			if (app->scene->winCondition) {
				this->state = EntityState::WIN;
			}

			if (isGrounded) {
				this->state = EntityState::IDLE;
			}

			Jump();

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

	//pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody = app->physics->CreateRectangle(position.x + 16, position.y + 16, 15, 25, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	//si quieres dar vueltos como la helice de un helicoptero Boeing AH-64 Apache pon en false la siguiente funcion
	pbody->body->SetFixedRotation(true);
	pbody->body->GetFixtureList()->SetFriction(20.0f);
	//pbody->body->SetLinearDamping(2);
	
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

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		direction = -1;
		state = EntityState::MOVE;
	}
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
		state = EntityState::JUMP;
	} 

	/*
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		if (pbody->body->GetLinearVelocity().x >= -5)
		{
			float impulse = pbody->body->GetMass() / 2;
			pbody->body->ApplyLinearImpulse({ -impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		if(pbody->body->GetLinearVelocity().x <= 5)
		{
			float impulse = pbody->body->GetMass() / 2;
			pbody->body->ApplyLinearImpulse({ impulse, 0 }, pbody->body->GetWorldCenter(), true);
		}
	}

			isGrounded = false;
		}
	}
	*/

	//si quieres dar putivueltas descomenta la linea de abajo y comenta la de arriba
	//app->render->DrawTexture(texture, position.x, position.y,0,1.0f,pbody->body->GetAngle()*RADTODEG);
	
	//app->render->DrawTexture(texture, position.x, position.y);//estoy hecho un lio, no se si esto va aqui o al final

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	//Esto esta aqui temporalmente don't worry :)
	//app->render->camera.x = -position.x + app->render->camera.w / app->win->GetScale() / 2;

	/* Info para el Hugo del futuro:
	
		1. el casteo de int es para que no se rompa al redondear
		2. el "-16" es el offset del tama�o del player
		3. el "4" que multiplica al "dt" es la "followSpeed"
	*/

	app->render->camera.x = std::ceil(std::lerp(app->render->camera.x, (int)(app->render->camera.w / 2 / app->win->GetScale()) - 16 - position.x, dt * 4 / 1000));
	//app->render->camera.x = std::ceil(std::lerp(app->render->camera.x, -position.x + 200, dt * 4 / 1000)); // esta funciona en escala 4 pero esta hardcodeado

	app->render->camera.y = -position.y + app->render->camera.h / app->win->GetScale() / 2;


	LOG("%d", -position.x);

	app->render->DrawTexture(texture, position.x, position.y);



	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
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

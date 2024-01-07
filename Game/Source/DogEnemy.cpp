#include "DogEnemy.h"
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

#include "Window.h"
#include <cmath>
#include <iostream>

#ifdef __linux__
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <SDL_render.h>
#endif

DogEnemy::DogEnemy() : Entity(EntityType::DOGENEMY)
{
	name.Create("DogEnemy");
	state = EntityState::IDLE;
}

DogEnemy::~DogEnemy() {

}

bool DogEnemy::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	newPosition = spawnPosition = position;

	// Load audios
	dogBark = app->audio->LoadFx("Assets/Audio/Fx/dogBark.wav");
	dogHit = app->audio->LoadFx("Assets/Audio/Fx/dogHit.wav");
	dogAttack = app->audio->LoadFx("Assets/Audio/Fx/DogAttack.ogg");
	dogDeath = app->audio->LoadFx("Assets/Audio/Fx/dogDeath.wav");
	
	return true;
}

bool DogEnemy::Start() {

	timer = Timer();
	timer.Start();

	movementDelay = Timer();
	timer.Start();

	player = app->scene->player;

	//load Animations
	idleAnim = *app->map->GetAnimByName("dog-idle-1");
	idleAnim.speed = 8.0f;
	runAnim = *app->map->GetAnimByName("dog-run-1");
	runAnim.speed = 8.0f;
	hurtAnim = *app->map->GetAnimByName("dog-hurt-1");
	hurtAnim.speed = 8.0f;
	hurtAnim.loop = false;

	dieAnim = *app->map->GetAnimByName("dog-die-1");
	dieAnim.speed = 8.0f;
	dieAnim.loop = false;

	currentAnimation = &idleAnim;
	
	pbody = app->physics->CreateRectangle(position.x, position.y, 20, 14, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	//si quieres dar vueltos como la helice de un helicoptero Boeing AH-64 Apache pon en false la siguiente funcion
	pbody->body->SetFixedRotation(true);
	pbody->body->GetFixtureList()->SetFriction(1.0f);
	pbody->body->SetLinearDamping(1);

	// TODO revisar este sensor, creo que no está funcionando
	groundSensor = app->physics->CreateRectangleSensor(position.x, position.y + pbody->width, 10, 5, bodyType::DYNAMIC);
	groundSensor->listener = this;

	return true;
}

bool DogEnemy::Update(float dt)
{

	// Update OwlEnemie state
	//StateMachine(dt);

	//Update OwlEnemie position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 24;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 17;

	// Update OwlEnemie sensors
	groundSensor->body->SetTransform({ pbody->body->GetPosition().x, pbody->body->GetPosition().y + PIXEL_TO_METERS(pbody->width) }, 0);


	// Render OwlEnemie texture
	app->render->DrawTexture(currentAnimation->texture, position.x, position.y - 12, &currentAnimation->GetCurrentFrame(), 1.0f, pbody->body->GetAngle()*RADTODEG, flip);

	currentAnimation->Update(dt);
	return true;
}

void DogEnemy::setIdleAnimation()
{
	currentAnimation = &idleAnim;
}

void DogEnemy::setMoveAnimation()
{
	currentAnimation = &runAnim;
	jumpAnim.Reset();
}

void DogEnemy::setJumpAnimation()
{
	currentAnimation = &jumpAnim;
}

void DogEnemy::Move(float dt) {
	// TODO move logic
}

void DogEnemy::Jump(float dt) {
	// TODO jump logic
}

bool DogEnemy::SaveState(pugi::xml_node& node) {

	pugi::xml_node dogEnemyAttributes = node.append_child("dogenemy");
	dogEnemyAttributes.append_attribute("x").set_value(this->position.x);
	dogEnemyAttributes.append_attribute("y").set_value(this->position.y);
	dogEnemyAttributes.append_attribute("angle").set_value(this->angle);
	dogEnemyAttributes.append_attribute("state").set_value((int)this->state);
	dogEnemyAttributes.append_attribute("lives").set_value(lives);

	return true;

}

bool DogEnemy::LoadState(pugi::xml_node& node)
{
	pugi::xml_node dogEnemyNode = node.child("dogenemy");

	pbody->body->SetTransform({ PIXEL_TO_METERS(dogEnemyNode.attribute("x").as_int()), PIXEL_TO_METERS(dogEnemyNode.attribute("y").as_int()) }, dogEnemyNode.attribute("angle").as_int());
	lives = dogEnemyNode.attribute("lives").as_int();
	this->state = (EntityState)dogEnemyNode.attribute("state").as_int();
	// reset enemy physics
	//pbody->body->SetAwake(false);
	//pbody->body->SetAwake(true);

	return true;
}

/* EntityState DogEnemy::StateMachine(float dt) {
	switch (this->state) {
	case EntityState::IDLE:
		setIdleAnimation();

		if (PIXEL_TO_METERS(player->position.DistanceTo(this->position)) < 3.0f)
		{
			state = EntityState::MOVE;
			// AUDIO DONE dog idle
			app->audio->PlayFx(dogBark);
		}
		break;
	case EntityState::MOVE:
		setMoveAnimation();
		pathfindingMovement(dt);
		if (PIXEL_TO_METERS(player->position.DistanceTo(this->position)) < 1.0f) {
			if (attackTimer.ReadSec() >= 2)
			{
				state = EntityState::ATTACK;
			}
		}
		else if ((PIXEL_TO_METERS(player->position.DistanceTo(this->position)) > 5.0f)) {
			moveToSpawnPoint();
			state = EntityState::IDLE;
		}
		break;
	case EntityState::DEAD:

		currentAnimation = &dieAnim;
		if (reviveTimer.ReadSec() >= 5)
		{
			state = EntityState::IDLE;
			lives = 5;
		}
		break;

	case EntityState::HURT:
		currentAnimation = &hurtAnim;
		invencible = true;
		if (currentAnimation->HasFinished()) {
			hurtAnim.Reset();
			hurtAnim.ResetLoopCount();
			invencible = false;
			state = EntityState::IDLE;
		}
		break;

	case EntityState::ATTACK:

		// AUDIO DONE dog attack
		app->audio->PlayFx(dogAttack);

		b2Vec2 attackDirection = { (float32)player->position.x - position.x, (float32)player->position.y - position.y };
		attackDirection.Normalize();

		b2Vec2 attackImpulse = { attackDirection.x / 4, attackDirection.y / 4 };

		pbody->body->ApplyLinearImpulse(attackImpulse, pbody->body->GetWorldCenter(), true);

		attackTimer.Start();

		state = EntityState::MOVE;
		break;
	}

	return this->state;
}
 */
void DogEnemy::moveToSpawnPoint()
{
	position = spawnPosition;

	pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0);

	// reset OwlEnemie physics
	pbody->body->SetAwake(false);
	pbody->body->SetAwake(true);
}

bool DogEnemy::CleanUp() {

	app->tex->UnLoad(texture);

	return true;
}

void DogEnemy::pathfindingMovement(float dt){

	iPoint origin = app->map->WorldToMap(newPosition.x + 8, newPosition.y + 8); //añadir el tile size / 2 hace que el owl se acerque mas

	if (timer.ReadMSec() > 250) {
		iPoint destination = app->map->WorldToMap(app->scene->player->position.x + 8, app->scene->player->position.y + 8);  //añadir el tile size / 2 hace que el owl se acerque mas
		app->map->pathfinding->CreatePath(origin, destination);
		timer.Start();
		currentPathPos = 0;
	}
	
	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();

	if (movementDelay.ReadMSec() > 100) {
		if (currentPathPos < path->Count())
		{
			newPosition = app->map->MapToWorld(path->At(currentPathPos)->x, path->At(currentPathPos)->y);
			currentPathPos++;
			movementDelay.Start();
		}
	}

	movementDirection = b2Vec2{(float32)newPosition.x, (float32)newPosition.y} - b2Vec2{(float32)position.x, (float32)position.y};
	movementDirection.Normalize();

	// Check if the dog is on the ground and apply force if it is not
	if (isGrounded) {
		if (abs(pbody->body->GetLinearVelocity().x) <= maxSpeed)
		{
			pbody->body->ApplyForce({ movementDirection.x * 1.5f, 0 }, pbody->body->GetWorldCenter(), true);
			if (pbody->body->GetLinearVelocity().x < -0.5f)
			{
				flip = SDL_FLIP_HORIZONTAL;
			}
			else if (pbody->body->GetLinearVelocity().x > 0.5f){
				flip = SDL_FLIP_NONE;
			}

			//flip = signbit(pbody->body->GetLinearVelocity().x) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			
		}
		canJump = true;
	} else {
		if (canJump) {
			canJump = false;
			pbody->body->ApplyLinearImpulse({ 0, movementDirection.y }, pbody->body->GetWorldCenter(), true);
		}
	}

	/*
	if (isGrounded and movementDirection.y != 0){
		float impulse = pbody->body->GetMass() * 5;
		pbody->body->ApplyLinearImpulse({0,impulse}, pbody->body->GetWorldCenter(), true);
	}
	*/
	


	LOG("%f, %f", movementDirection);

	//LOG("%d, %d", pbody->body->GetPosition().x, pbody->body->GetPosition().y);


	if (debug)
	{
		//Render Path
		if (path->Count() > 0)
		{
			for (uint i = 0; i < path->Count() - 1; ++i)
			{
				iPoint pos1 = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				iPoint pos2 = app->map->MapToWorld(path->At(i + 1)->x, path->At(i + 1)->y);
				app->render->DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, 0, 0, 255);
			}

		}

	//Debug: Render the line between Owl and Player
	app->render->DrawLine(position.x + 27, position.y + 17, app->scene->player->position.x + 20, app->scene->player->position.y + 10, 0, 0, 255);
	}
}

void DogEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	if (physA->body->GetFixtureList()->IsSensor()) {
		if (physB->ctype == ColliderType::PLATFORM) {
			if (physA == groundSensor) {
				LOG("Ground collision");
				isGrounded = true;
			}
		}
	}

	switch (physB->ctype) {

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;

	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::DEATH:
		LOG("Collision DEATH");
		app->entityManager->DestroyEntity(this);
		break;
	case ColliderType::BULLET:
		LOG("Collision DEATH");
		if (state != EntityState::DEAD and !invencible){
			if (lives <= 1)
			{
				// AUDIO DONE dog death
				app->audio->PlayFx(dogDeath);
				state = EntityState::DEAD;
				reviveTimer.Start();
			}
			else{
				// AUDIO DONE dog hit
				app->audio->PlayFx(dogHit);
				state = EntityState::HURT;
				lives--;
			}
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

void DogEnemy::EndCollision(PhysBody* physA, PhysBody* physB) {
	if (physA->body->GetFixtureList()->IsSensor()) {
		if (physB->ctype == ColliderType::PLATFORM) {
			if (physA == groundSensor) {
				LOG("Ground collision");
				isGrounded = false;
			}
		}
	}
}

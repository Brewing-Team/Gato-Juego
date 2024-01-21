#include "DogEnemyHurtState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void DogEnemyHurtState::Enter()
{
    LOG("DogEnemyHurtState::Enter()\n");
    dogEnemy = StateMachineReference->owner;
	dogEnemy->currentAnimation = &dogEnemy->hurtAnim;
}

void DogEnemyHurtState::Update(float dt)
{
	dogEnemy->invencible = true;
	if (dogEnemy->currentAnimation->HasFinished()) {
		dogEnemy->hurtAnim.Reset();
		dogEnemy->hurtAnim.ResetLoopCount();
		dogEnemy->invencible = false;
		StateMachineReference->ChangeState("idle");
	}
}

void DogEnemyHurtState::Exit()
{
    LOG("DogEnemyHurtState::Exit()\n");
}


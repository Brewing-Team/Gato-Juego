#include "OwlEnemyHurtState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void OwlEnemyHurtState::Enter()
{
    LOG("OwlEnemyHurtState::Enter()\n");
    owlEnemy = StateMachineReference->owner;
    owlEnemy->currentAnimation = &owlEnemy->hurtedAnim;
    owlEnemy->invencible = true;
}

void OwlEnemyHurtState::Update(float dt)
{
		if ( owlEnemy->currentAnimation->HasFinished()) {
			owlEnemy->hurtedAnim.Reset();
			owlEnemy->hurtedAnim.ResetLoopCount();
			owlEnemy->invencible = false;
			StateMachineReference->ChangeState("move");
		}
}

void OwlEnemyHurtState::Exit()
{
    LOG("OwlEnemyHurtState::Exit()\n");
}


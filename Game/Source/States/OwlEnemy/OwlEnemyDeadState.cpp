#include "OwlEnemyDeadState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void OwlEnemyDeadState::Enter()
{
    LOG("OwlEnemyDeadState::Enter()\n");
    owlEnemy = StateMachineReference->owner;
    owlEnemy->currentAnimation = &owlEnemy->sleepingAnim;
    owlEnemy->pbody->body->SetFixedRotation(false);
    owlEnemy->pbody->body->SetGravityScale(1);
}

void OwlEnemyDeadState::Update(float dt)
{
		if (owlEnemy->reviveTimer.ReadSec() >= 5)
		{
			owlEnemy->pbody->body->SetFixedRotation(true);
			owlEnemy->pbody->body->SetGravityScale(0);
			StateMachineReference->ChangeState("idle");
			owlEnemy->lives = 3;
		}
}

void OwlEnemyDeadState::Exit()
{
    LOG("OwlEnemyDeadState::Exit()\n");
}


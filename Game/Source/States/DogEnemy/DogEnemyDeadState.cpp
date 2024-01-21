#include "DogEnemyDeadState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void DogEnemyDeadState::Enter()
{
    LOG("DogEnemyDeadState::Enter()\n");
    dogEnemy = StateMachineReference->owner;
	dogEnemy->currentAnimation = &dogEnemy->dieAnim;
}

void DogEnemyDeadState::Update(float dt)
{
	if (dogEnemy->reviveTimer.ReadSec() >= 5)
	{
		StateMachineReference->ChangeState("idle");
		dogEnemy->lives = 5;
	}
}

void DogEnemyDeadState::Exit()
{
    LOG("DogEnemyDeadState::Exit()\n");
}


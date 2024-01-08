#include "PlayerHurtState.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerHurtState::Enter()
{
    LOG("PlayerHurtState::Enter()\n");
    player = StateMachineReference->owner;
    player->currentAnimation = &player->hurtAnim;
}

void PlayerHurtState::Update(float dt)
{
    LOG("PlayerHurtState::Update()\n");
    if (player->currentAnimation->HasFinished()) {
        player->hurtAnim.Reset();
        player->hurtAnim.ResetLoopCount();
        StateMachineReference->ChangeState("idle");
    }
}

void PlayerHurtState::Exit()
{
    LOG("PlayerHurtState::Exit()\n");
}


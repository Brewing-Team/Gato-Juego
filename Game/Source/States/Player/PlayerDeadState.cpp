#include "PlayerDeadState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerDeadState::Enter()
{
    LOG("PlayerDeadState::Enter()\n");
    player = StateMachineReference->owner;
}

void PlayerDeadState::Update(float dt)
{
    LOG("PlayerDeadState::Update()\n");
    player->setWinAnimation();

    // TODO resetear mundo, restar vidas, etc

    // AUDIO DONE player death
    app->audio->PlayFx(player->playerDeath);

    player->moveToSpawnPoint();

    player->isAlive = true;
    player->lives = 7;
    StateMachineReference->ChangeState("idle");
}

void PlayerDeadState::Exit()
{
    LOG("PlayerDeadState::Exit()\n");
}


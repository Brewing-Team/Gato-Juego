#include "PlayerWinState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerWinState::Enter()
{
    LOG("PlayerWinState::Enter()\n");
    player = StateMachineReference->owner;
}

void PlayerWinState::Update(float dt)
{
    LOG("PlayerWinState::Update()\n");
    app->audio->PlayFx(player->playerWin);

    player->moveToSpawnPoint();

	StateMachineReference->ChangeState("idle");
}

void PlayerWinState::Exit()
{
    LOG("PlayerWinState::Exit()\n");
}


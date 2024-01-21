#include "PlayerNoClipState.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerNoClipState::Enter()
{
    LOG("PlayerNoClipState::Enter()\n");
    player = StateMachineReference->owner;
}

void PlayerNoClipState::Update(float dt)
{
    LOG("PlayerNoClipState::Update()\n");

    player->setIdleAnimation();
    
    if (player->noClip) {
        // deactivate physics
        player->pbody->body->SetAwake(false);

        // Deactivate gravity
        player->pbody->body->SetGravityScale(0);

        // debug controls
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
            player->pbody->body->SetLinearVelocity({ -3,0 });
        }

        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
            player->pbody->body->SetLinearVelocity({ 3,0 });
        }

        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
            player->pbody->body->SetLinearVelocity({ 0,-3 });
        }

        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
            player->pbody->body->SetLinearVelocity({ 0,3 });
        }
    }
    else {
        // activate physics
        player->pbody->body->GetFixtureList()->SetSensor(false);
        player->pbody->body->SetAwake(true);

        // Activate Gravity
        player->pbody->body->SetGravityScale(1);

        StateMachineReference->ChangeState("idle");
    }

}

void PlayerNoClipState::Exit()
{
    LOG("PlayerNoClipState::Exit()\n");
}


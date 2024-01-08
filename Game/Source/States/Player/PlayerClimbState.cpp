#include "PlayerClimbState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerClimbState::Enter()
{
    LOG("PlayerClimbState::Enter()\n");
    player = StateMachineReference->owner;
}

void PlayerClimbState::Update(float dt)
{
    LOG("PlayerClimbState::Update()\n");
    
    if (player->isGrounded)
		{
			if ((int)player->pbody->body->GetLinearVelocity().y == 0) //esto rarete pero buenop
			{
				player->setIdleAnimation();
			}
			else {
				player->setClimbAnimation();
			}
		}
		else
		{
			player->setJumpAnimation();
		}


		if (!player->isAlive and !player->godMode) {
			//this->state = EntityState::DEAD;
		}

		if (app->scene->winCondition) {
			//this->state = EntityState::WIN;
		}

		if (!player->isGrounded and !player->isCollidingLeft and !player->isCollidingRight) {
			//this->state = EntityState::IDLE;
            StateMachineReference->ChangeState("idle");
			player->angle = 0; //temporal, esto deberia ir en el enter del state idle
		}

		player->Climb(dt);
}

void PlayerClimbState::Exit()
{
    LOG("PlayerClimbState::Exit()\n");
}


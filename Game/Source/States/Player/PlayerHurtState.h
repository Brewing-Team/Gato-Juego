#ifndef __PLAYERHURTSTATE_H__
#define __PLAYERHURTSTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerHurtState : public State<Player> {
    public:
    PlayerHurtState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERHURTSTATE_H__
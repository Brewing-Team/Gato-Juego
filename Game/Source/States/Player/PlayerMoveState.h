#ifndef __PLAYERMOVESTATE_H__
#define __PLAYERMOVESTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerMoveState : public State<Player> {
    public:
    PlayerMoveState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERMOVESTATE_H__
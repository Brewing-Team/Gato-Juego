#ifndef __PLAYERCLIMBSTATE_H__
#define __PLAYERCLIMBSTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerClimbState : public State<Player> {
    public:
    PlayerClimbState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERCLIMBSTATE_H__
#ifndef __PLAYERNOCLIPSTATE_H__
#define __PLAYERNOCLIPSTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerNoClipState : public State<Player> {
    public:
    PlayerNoClipState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERNOCLIPSTATE_H__
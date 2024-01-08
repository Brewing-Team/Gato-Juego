#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class IdleState : public State<Player> {
    public:
    IdleState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class IdleState : public State<Player> {
    public:
    IdleState(SString name);
    void Enter() override;
    void PreUpdate() override;
    void Update(float dt) override;
    void PostUpdate() override;
    void Exit() override;
};

#ifndef __DOGENEMYDEADSTATE_H__
#define __DOGENEMYDEADSTATE_H__

#include "DogEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class DogEnemyDeadState : public State<DogEnemy> {
    public:
    DogEnemyDeadState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    DogEnemy* dogEnemy;
};
#endif // __DOGENEMYDEADSTATE_H__

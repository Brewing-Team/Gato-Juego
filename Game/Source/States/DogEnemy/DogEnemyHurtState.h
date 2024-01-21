#ifndef __DOGENEMYHURTSTATE_H__
#define __DOGENEMYHURTSTATE_H__

#include "DogEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class DogEnemyHurtState : public State<DogEnemy> {
    public:
    DogEnemyHurtState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    DogEnemy* dogEnemy;
};
#endif // __DOGENEMYHURTSTATE_H__

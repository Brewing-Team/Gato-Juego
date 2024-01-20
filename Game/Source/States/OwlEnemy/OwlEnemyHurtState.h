#ifndef __OWLENEMYHURTSTATE_H__
#define __OWLENEMYHURTSTATE_H__

#include "OwlEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class OwlEnemyHurtState : public State<OwlEnemy> {
    public:
    OwlEnemyHurtState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    OwlEnemy* owlEnemy;
};
#endif // __OWLENEMYHURTSTATE_H__

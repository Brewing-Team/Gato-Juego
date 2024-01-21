#ifndef __DOGENEMYMOVESTATE_H__
#define __DOGENEMYMOVESTATE_H__

#include "DogEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class DogEnemyMoveState : public State<DogEnemy> {
    public:
    DogEnemyMoveState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    DogEnemy* dogEnemy;
};
#endif // __DOGENEMYMOVESTATE_H__

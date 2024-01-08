#ifndef __STATE_H__
#define __STATE_H__
#include "SString.h"
#include "StateMachine.h"

template <typename T>
class State {
    public:
    State() {}
    State(SString name) : name(name) {}
    virtual void Enter() {}
    virtual void PreUpdate() {}
    virtual void Update(float dt) {}
    virtual void PostUpdate() {}
    virtual void Exit() {}

    SString name;
    StateMachine<T>* StateMachineReference;
};
#endif // __STATE_H__
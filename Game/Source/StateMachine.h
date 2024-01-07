#include "DynArray.h"
#include "Entity.h"
#include "State.h"
#include "SString.h"
#include <vector>

class StateMachine {
    public:
    StateMachine(Entity* owner);
    ~StateMachine();
    void PreUpdate();
    void Update(float dt);
    void PostUpdate();
    void ChangeState(SString stateName);
    void AddState(State* newState);
    State* FindState(SString stateName);
    private:
    State* currentState;
    DynArray<State*> states; //ver si tienen que ser punteros o no
    Entity* owner;
};
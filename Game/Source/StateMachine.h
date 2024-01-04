#include "DynArray.h"
#include "State.h"
#include "SString.h"
#include <vector>

class StateMachine {
    public:
    StateMachine();
    ~StateMachine();
    void PreUpdate();
    void Update();
    void PostUpdate();
    void ChangeState(SString stateName);
    void AddState(State* newState);
    State* FindState(SString stateName);
    private:
    State* currentState;

   DynArray<State*> states; //ver si tienen que ser punteros o no
};
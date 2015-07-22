#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include <unordered_map>
#include <queue>

class InputComponent {
public:
     bool
     w{false},
     a{false},
     s{false},
     d{false},
     space{false};
     std::unordered_map<int, int> bindings; //The key to bindings is a keyboard-key, the value is an action (enum)
     std::queue<int> presses; //Filled with whatever keyes that was pressed a tick
     std::queue<int> releases; //Filled with whatever keyes that was released a tick
};

#endif //INPUTCOMPONENT_H

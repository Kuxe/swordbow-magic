#ifndef KEYBOARDSYSTEM_HPP
#define KEYBOARDSYSTEM_HPP

#include <SDL2/SDL.h>
#include <string>
#include "isystem.hpp"

typedef unsigned int ID;

class InputSystem;
class KeyboardSystem : public ISystem {
private:
    SDL_Event event;
    bool* runningPtr;
    ID id; //Id that is controlled by keyboard
public:
    KeyboardSystem(bool* runningPtr);
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const std::string getIdentifier() const;
    void activateId(ID id);
};

#endif //KEYBOARDSYSTEM_HPP

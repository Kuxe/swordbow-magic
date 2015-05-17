#ifndef CAMERASYSTEM_HPP
#define CAMERASYSTEM_HPP

#include <SDL2/SDL.h>
#include <unordered_set>
#include "isystem.hpp"

using std::unordered_set;

typedef unsigned int ID;
class RenderSystem;
class CameraSystem : public ISystem {
private:
    ID cameraSource;
    RenderSystem* renderSystem;
    SDL_Rect camera;

public:
    CameraSystem(RenderSystem* renderSystem);
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
    void setRenderSystem(RenderSystem* renderSystem);
    SDL_Rect& getCamera();
};


#endif //CAMERASYSTEM_HPP

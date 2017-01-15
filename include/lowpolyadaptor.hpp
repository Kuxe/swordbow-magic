#ifndef LOWPOLYADAPTOR_HPP
#define LOWPOLYADAPTOR_HPP

#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "text.hpp"
#include "imageidentifiers.hpp"
#include "overlay.hpp"
#include "irenderer.hpp"

//TODO: Use lowpoly3d
/** If this class is an interface, IRenderer, then there can be another class called Lowpoly3dRenderer which realizes this interface.
    Lowpoly3dRenderer in turn has a member of type lowpoly3d::Renderer. This architecture means that I can if ever redoing renderer
    just swap out the Lowpoly3dRenderer for some other NextgenRenderer without ever redoing anything within swordbow-magic
    which is nice. I think this is the adaptor-pattern. **/


class LowpolyAdaptor : public IRenderer {
private:
    std::queue<Text> texts;
    void renderTexts();
public:
    LowpolyAdaptor(bool fullscreen, bool vsync);
    ~LowpolyAdaptor();
    
    //Difference between renderOverlays and renderOnlyOverlays is that
    //renderOverlays doesn't call RenderPresent, it calls RenderCopy and copies
    //overlays onto overlayTexture while renderOnlyOverlays does that AND
    //clear default-texture and renders it (clear everything + draw only overlay)
    void render();
    void renderOverlays();
    void renderOnlyOverlays();
    void showOverlay(const Image::Identifier& identifier, const Text& text = {"", glm::ivec2(0.0), glm::vec3(0.0)});
    void fadeInOverlay(const Image::Identifier& identifier, float seconds, const Text& text = {"", glm::ivec2(0.0), glm::vec3(0.0)});
    void hideOverlay(const Image::Identifier& identifier);
    void fadeOutOverlay(const Image::Identifier& identifier, float seconds);
    void printText(const Text& text);
    virtual glm::ivec2 getWindowResolution() { return {0, 0}; } //TODO: Implement
    void pollEvents(IClientState* clientState);
};


#endif //LOWPOLYADAPTOR_HPP

#ifndef LOWPOLYADAPTOR_HPP
#define LOWPOLYADAPTOR_HPP

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>

#include "text.hpp"
#include "imageidentifiers.hpp"
#include "overlay.hpp"
#include "irenderer.hpp"
#include "lowpoly3d.hpp"
#include "renderquerier.hpp"
#include "ilowpolyinput.hpp"

#include "events.hpp" //Events which swordbow-magic (IClientState) can handle

#include <glm/mat4x4.hpp>

/** If this class is an interface, IRenderer, then there can be another class called Lowpoly3dRenderer which realizes this interface.
    Lowpoly3dRenderer in turn has a member of type lowpoly3d::Renderer. This architecture means that I can if ever redoing renderer
    just swap out the Lowpoly3dRenderer for some other NextgenRenderer without ever redoing anything within swordbow-magic
    which is nice. I think this is the adaptor-pattern. **/

class LowpolyAdaptor :
    public IRenderer,
    public lowpoly3d::RenderQuerier,
    public lowpoly3d::ILowpolyInput {
private:
    std::queue<Text> texts;
    lowpoly3d::Renderer renderer;
    std::vector<lowpoly3d::RenderData> rds;
    glm::mat4 view;

    /** Typed event queues. I could probably use templates here to
        get rid of linear increase in code with events but the number
        of different events lowpoly sends is manageable as of 2017-03-20 **/
    std::vector<MouseEvent> mouseEvents;
    std::vector<KeyEvent> keyEvents;

    void renderTexts();
public:
    LowpolyAdaptor(bool fullscreen, bool vsync);
    ~LowpolyAdaptor();

    /** Since lowpoly3d is blocking, whatever thread that calls run will be blocked
        until window is closed **/
    void run();
    
    /** Methods required by IRenderer **/
    //Difference between renderOverlays and renderOnlyOverlays is that
    //renderOverlays doesn't call RenderPresent, it calls RenderCopy and copies
    //overlays onto overlayTexture while renderOnlyOverlays does that AND
    //clear default-texture and renders it (clear everything + draw only overlay)
    void render(const std::vector<ID>& activeIds, const ComponentManager& cm, const glm::mat4& view);
    void renderOverlays();
    void renderOnlyOverlays();
    void showOverlay(const Image::Identifier& identifier, const Text& text = {"", glm::ivec2(0.0), glm::vec3(0.0)});
    void fadeInOverlay(const Image::Identifier& identifier, float seconds, const Text& text = {"", glm::ivec2(0.0), glm::vec3(0.0)});
    void hideOverlay(const Image::Identifier& identifier);
    void fadeOutOverlay(const Image::Identifier& identifier, float seconds);
    void printText(const Text& text);
    virtual glm::ivec2 getWindowResolution() { return {0, 0}; } //TODO: Implement
    void pollEvents(IClientState* clientState);
    /** End of methods required by IRenderer **/

    /** Methods required by RenderQuerier **/
    const std::vector<lowpoly3d::RenderData>& getRenderDatas() const;
    const glm::mat4 getView() const;
    const float getGametime() const;
    const float getSunRadians() const;
    /** End of methods required by RenderQuerier **/

    /** Methods required by ILowpolyInput **/
    void onKey(int key, int scancode, int action, int mods);
    void onMouse(double xpos, double ypos);
    /** End of methods required by ILowpolyInput **/
};


#endif //LOWPOLYADAPTOR_HPP

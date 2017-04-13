#ifndef IRENDERER_HPP
#define IRENDERER_HPP

#include <glm/glm.hpp>
#include <vector>
#include "idmanager.hpp" //for definition of ID
#include <glm/mat4x4.hpp>

//Forward declarations
namespace Image { enum Identifier; }
class Text;
class IClientState;
class ComponentManager;

/** IRenderer is an interface used by swordbow-magic client. IRenderer provides various graphical
	functionality that the swordbow-magic client may need. **/
struct IRenderer {
	virtual ~IRenderer() { };

    /** Render causes the 3rd party api to trigger a render. All things that 3rd api may need
        for rendering should be passed as argument here. ActiveIDs and ComponentManager are 
        required since swordbow-magic must be able to convert MoveComponent and RenderComponent
        into some datatype used by 3rd party library. The view matrix is also required for otherwise
        there would be now way of telling the renderer from what viewpoint the scene should be rendered. **/
	virtual void render(const std::vector<ID>& activeIds, const ComponentManager& cm, const glm::mat4& view) = 0;
    virtual void renderOverlays() = 0;
    virtual void renderOnlyOverlays() = 0;
    virtual void showOverlay(const Image::Identifier& identifier, const Text& text) = 0;
    virtual void fadeInOverlay(const Image::Identifier& identifier, float seconds, const Text& text) = 0;
    virtual void hideOverlay(const Image::Identifier& identifier) = 0;
    virtual void fadeOutOverlay(const Image::Identifier& identifier, float seconds) = 0;
    virtual void printText(const Text& text) = 0;
    virtual glm::ivec2 getWindowResolution() = 0;
    virtual void pollEvents(IClientState* const clientState) = 0; //The renderer must be able to tell the client about events
};

#endif //IRENDERER_HPP
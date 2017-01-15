#ifndef IRENDERER_HPP
#define IRENDERER_HPP

#include <glm/glm.hpp>

//Forward declarations
namespace Image { enum Identifier; }
class Text;
class IClientState;

/** IRenderer is an interface used by swordbow-magic client. IRenderer provides various graphical
	functionality that the swordbow-magic client may need. **/
struct IRenderer {
	virtual ~IRenderer() { };
	virtual void render() = 0;
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
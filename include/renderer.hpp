#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "text.hpp"
#include "texturedata.hpp"
#include "imageidentifiers.hpp"
#include "overlay.hpp"

//TODO: Use lowpoly3d
/** If this class is an interface, IRenderer, then there can be another class called Lowpoly3dRenderer which realizes this interface.
    Lowpoly3dRenderer in turn has a member of type lowpoly3d::Renderer. This architecture means that I can if ever redoing renderer
    just swap out the Lowpoly3dRenderer for some other NextgenRenderer without ever redoing anything within swordbow-magic
    which is nice. I think this is the adaptor-pattern. **/


class Renderer {
private:
    std::queue<Text> texts;

    static constexpr uint16_t SCREEN_WIDTH = 1920 / 4;
	static constexpr uint16_t SCREEN_HEIGHT = 1080 / 4;

    void renderTexts();
public:
    Renderer(bool fullscreen, bool vsync);
    ~Renderer();
    const std::unordered_map<Image::Identifier, TextureData, std::hash<int>>& getTextureDatas() const;
    
    //Difference between renderOverlays and renderOnlyOverlays is that
    //renderOverlays doesn't call RenderPresent, it calls RenderCopy and copies
    //overlays onto overlayTexture while renderOnlyOverlays does that AND
    //clear default-texture and renders it (clear everything + draw only overlay)
    void renderOverlays();
    void renderOnlyOverlays();
    void showOverlay(Image::Identifier identifier, Text text = {"", 0, 0});
    void fadeInOverlay(Image::Identifier identifier, float seconds, Text text = {"", 0, 0});
    void hideOverlay(Image::Identifier identifier);
    void fadeOutOverlay(Image::Identifier identifier, float seconds);

    void printText(const Text& text);
    static constexpr uint16_t getScreenWidth() { return SCREEN_WIDTH; }
    static constexpr uint16_t getScreenHeight() { return SCREEN_HEIGHT; }
};


#endif //RENDERER_HPP

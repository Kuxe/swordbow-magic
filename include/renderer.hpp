#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "text.hpp"
#include "texturedata.hpp"
#include "renderdata.hpp"
#include "imageidentifiers.hpp"
#include "overlay.hpp"

class Renderer {
private:
    std::queue<Text> texts;

    static constexpr ushort SCREEN_WIDTH = 1920 / 4;
	static constexpr ushort SCREEN_HEIGHT = 1080 / 4;

	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;

    //WorldTexture is rendered fisrt, then fontTexture, then overlayTexture and finally textOverlayTexture
	SDL_Texture* worldTexture = nullptr;
	SDL_Texture* fontTexture = nullptr;
    SDL_Texture* overlayTexture = nullptr;


	TTF_Font* font;
	std::unordered_map<Image::Identifier, TextureData, std::hash<int>> textureDatas;
    std::unordered_map<Image::Identifier, Overlay, std::hash<int>> overlays;

    void renderTexts();

public:
    Renderer(int argc, char** argv);
    ~Renderer();
    const std::unordered_map<Image::Identifier, TextureData, std::hash<int>>& getTextureDatas() const;
    void render(std::priority_queue<RenderData>& pq, const SDL_Rect& camera);
    
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
    static constexpr ushort getScreenWidth() { return SCREEN_WIDTH; }
    static constexpr ushort getScreenHeight() { return SCREEN_HEIGHT; }

};

#endif //RENDERER_HPP

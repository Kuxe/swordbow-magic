#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <unordered_map>
#include <queue>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "text.hpp"
#include "texturedata.hpp"
#include "renderdata.hpp"
#include "imageidentifiers.hpp"

class Renderer {
private:
    std::queue<Text> texts;

    static constexpr ushort SCREEN_WIDTH = 1920 / 4;
	static constexpr ushort SCREEN_HEIGHT = 1080 / 4;

	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Texture* worldTexture = nullptr;
	SDL_Texture* fontTexture = nullptr;
	TTF_Font* font;
	std::unordered_map<Image::Identifier, TextureData, std::hash<int>> textureDatas;

    void renderTexts();

public:
    Renderer(int argc, char** argv);
    ~Renderer();
    const std::unordered_map<Image::Identifier, TextureData, std::hash<int>>& getTextureDatas() const;
    void render(std::priority_queue<RenderData>& pq, const SDL_Rect& camera);
    void printText(const Text& text);
    static constexpr ushort getScreenWidth() { return SCREEN_WIDTH; }
    static constexpr ushort getScreenHeight() { return SCREEN_HEIGHT; }
};

#endif //RENDERER_HPP

#ifndef TEXTUREDATA_HPP
#define TEXTUREDATA_HPP

class SDL_Teture;
struct TextureData {
    SDL_Texture* texture;
	unsigned int width;
	unsigned int height;

    constexpr TextureData() :
        texture(nullptr),
        width(0),
        height(0) { }

    constexpr TextureData(
        SDL_Texture* texture,
        const unsigned int width,
        const unsigned int height
    ) :
    texture(texture),
    width(width),
    height(height) { }

    constexpr TextureData(const TextureData& rhs) :
        texture(rhs.texture),
        width(rhs.width),
        height(rhs.height) { }
};


#endif //TEXTUREDATA_HPP

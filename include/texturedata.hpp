#ifndef TEXTUREDATA_HPP
#define TEXTUREDATA_HPP

#include <glm/vec2.hpp>

class SDL_Texture;
struct TextureData {
    SDL_Texture* texture;
	glm::vec2 dimension;

    TextureData() :
        texture(nullptr),
        dimension(0.0f, 0.0f) { }

    TextureData(
        SDL_Texture* texture,
        const glm::vec2& dimension
    ) :
    texture(texture),
    dimension(dimension) { }

    TextureData(const TextureData& rhs) :
        texture(rhs.texture),
        dimension(rhs.dimension) { }
};


#endif //TEXTUREDATA_HPP

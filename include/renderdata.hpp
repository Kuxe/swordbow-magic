#ifndef RENDERDATA_HPP
#define RENDERDATA_HPP

#include "texturedata.hpp"

struct RenderData {

	//TODO: make members const, need to sort out
	//implicitly deleted copy-assignment before that
	TextureData textureData;
	SDL_Rect cliprect;
	SDL_Rect target;
    int zindex_base;
    int zindex;

    constexpr RenderData(
        const TextureData& textureData,
        const SDL_Rect& cliprect,
        const SDL_Rect& target,
        const int zindex_base,
        const int zindex
    ) :
    textureData(textureData),
    cliprect(cliprect),
    target(target),
    zindex_base(zindex_base),
    zindex(zindex) { }

    constexpr RenderData(const RenderData& rhs) :
        RenderData(rhs.textureData,
        rhs.cliprect,
        rhs.target,
        rhs.zindex_base,
        rhs.zindex) { }

		constexpr RenderData() :
		RenderData{
			{nullptr, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			(0),
			(0)
		} { }

    constexpr bool operator< (const RenderData& rhs) const {
        return
            zindex_base == rhs.zindex_base ?
            zindex < rhs.zindex :
            zindex_base < rhs.zindex_base;
    }
    constexpr bool operator> (const RenderData& rhs) const { return rhs < *this; }
    constexpr bool operator<=(const RenderData& rhs) const { return !(*this > rhs); }
    constexpr bool operator>=(const RenderData& rhs) const { return !(*this < rhs); }
};



#endif //RENDERDATA_HPP

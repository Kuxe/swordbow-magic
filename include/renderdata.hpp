#ifndef RENDERDATA_HPP
#define RENDERDATA_HPP

#include <string>

struct RenderData {

	std::string texturePath;
	SDL_Rect cliprect;
	SDL_Rect target;
    int zindex_base;
    int zindex;

    RenderData(
        const string& texturePath,
        const SDL_Rect& cliprect,
        const SDL_Rect& target,
        const int zindex_base,
        const int zindex
    ) :
	texturePath(texturePath),
    cliprect(cliprect),
    target(target),
    zindex_base(zindex_base),
    zindex(zindex) { }

    RenderData(const RenderData& rhs) :
        RenderData(rhs.texturePath,
        rhs.cliprect,
        rhs.target,
        rhs.zindex_base,
        rhs.zindex) { }

	RenderData() :
		RenderData{
			{"NO TEXTUREPATH SET", 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			(0),
			(0)
		} { }

    constexpr bool operator< (const RenderData& rhs) const {
        return
            zindex_base == rhs.zindex_base ?
            zindex > rhs.zindex :
            zindex_base > rhs.zindex_base;
    }
    constexpr bool operator> (const RenderData& rhs) const { return rhs < *this; }
    constexpr bool operator<=(const RenderData& rhs) const { return !(*this > rhs); }
    constexpr bool operator>=(const RenderData& rhs) const { return !(*this < rhs); }
};



#endif //RENDERDATA_HPP

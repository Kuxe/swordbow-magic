#ifndef RECT_HPP
#define RECT_HPP
#include <ostream>

struct Rect {
    float x, y;
    float w, h;

    constexpr inline bool operator==(const Rect& rhs) const {
        return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
    }
    constexpr inline bool operator!=(const Rect& rhs) const {
        return !(*this == rhs);
    }

    /**
     *	Bounding-box test
     **/
    static constexpr inline const bool intersect(const Rect& a, const Rect& b) {

        //Im sure it should be <=.. not <.
        //Draw a picture on a squared paper if in doubt...
        return !(
                a.x + a.w <= b.x ||
                a.x >= b.x + b.w ||
                a.y + a.h <= b.y ||
                a.y >= b.y + b.h);
    }

    static constexpr inline const Rect getIntersection(const Rect& a, const Rect& b) {
        //TODO: This can probably be optimized
        const float x = a.x > b.x ? a.x : b.x;
        const float y = a.y > b.y ? a.y : b.y;
        const float w = (a.x + a.w) < (b.x + b.w) ? a.x + a.w - x : b.x + b.w - x;
        const float h = (a.y + a.h) < (b.y + b.h) ? a.y + a.h - y : b.y + b.h - y;
        return {x, y, w, h};
    }

    friend std::ostream& operator<< (std::ostream& stream, const Rect& rect);
};




#endif //RECT_HPP

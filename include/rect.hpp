#ifndef RECT_HPP
#define RECT_HPP
#include <ostream>
#include <cmath>

struct Rect {
    float x, y, w, h;

    inline bool operator==(const Rect& rhs) const {
        constexpr float EPSILON = 0.00001f;
        return
            std::fabs(x - rhs.x) < EPSILON &&
            std::fabs(y - rhs.y) < EPSILON &&
            std::fabs(w - rhs.w) < EPSILON &&
            std::fabs(h - rhs.h) < EPSILON;
    }
    inline bool operator!=(const Rect& rhs) const {
        return !(*this == rhs);
    }

    /**
     *	Bounding-box test
     **/
    static inline const bool intersect(const Rect& a, const Rect& b) {

        //Im sure it should be <=.. not <.
        //Draw a picture on a squared paper if in doubt...
        return !(
                a.x + a.w <= b.x ||
                a.x >= b.x + b.w ||
                a.y + a.h <= b.y ||
                a.y >= b.y + b.h);
    }

    static inline const float& max(const float& a, const float& b) {
        return a > b ? a : b;
    }

    static inline const float& min(const float& a, const float& b) {
        return a < b ? a : b;
    }

    //Returns {0, 0, 0, 0} if there was no intersection
    static inline const Rect getIntersection(const Rect& a, const Rect& b) {
        //If a intersects b...
        return intersect(a, b) ?

        //Then return intersection area
            Rect{
                max(a.x, b.x),
                max(a.y, b.y),
                min(a.x + a.w, b.x + b.w) - max(a.x, b.x),
                min(a.y + a.h, b.y + b.h) - max(a.y, b.y)
            }

        //Otherwise return {0, 0, 0, 0}
            : Rect{0, 0, 0, 0};
    }

    friend std::ostream& operator<< (std::ostream& stream, const Rect& rect);
};




#endif //RECT_HPP

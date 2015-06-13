#include "rect.hpp"

std::ostream& operator<< (std::ostream& stream, const Rect& rect) {
    return stream << "{" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << "}";
}

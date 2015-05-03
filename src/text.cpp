#include "text.hpp"

Text::Text(const string& text, const int& x, const int& y, const SDL_Color& color) :
    text(text),
    x(x),
    y(y),
    color(color) {
}

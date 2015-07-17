#include "text.hpp"

Text::Text(const std::string& text, int x, int y, const SDL_Color& color) :
    text(text),
    x(x),
    y(y),
    color(color) {
}

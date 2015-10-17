#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL_ttf.h>
#include <string>

class Text {
public:
    const std::string text;
    const SDL_Color color;
    const int x, y;
    Text(const std::string& text, int x, int y, const SDL_Color& color = {255, 255, 255, 255});
};




#endif //TEXT_H

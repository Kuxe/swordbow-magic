#ifndef TEXT_H
#define TEXT_H

//TODO: Use lowpoly3d
#include <string>

class Text {
public:
    const std::string text;
    //const SDL_Color color;
    const int x, y;
    Text(const std::string& text, int x, int y);
};




#endif //TEXT_H

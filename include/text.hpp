#ifndef TEXT_H
#define TEXT_H

#include <glm/glm.hpp>
#include <string>

class Text {
public:
    const std::string text;
    const glm::ivec2 pos; //Top-left position of text in screen-space
    const glm::vec3 color;
    Text(const std::string& text, const glm::ivec2& pos, const glm::vec3& color);
};




#endif //TEXT_H

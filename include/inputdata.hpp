#ifndef INPUTDATA_HPP
#define INPUTDATA_HPP

#include <vector>
#include <glm/vec2.hpp>
#include "renderdata.hpp" //For glm cereal serialize

struct InputData {
    std::vector<int> presses; //Filled with whatever keyes that was pressed a tick
    std::vector<int> releases; //Filled with whatever keyes that was released a tick
    glm::vec2 mousePos; //Most recent mouse position

    template<class Archive>
    void serialize(Archive& ar) {
        ar(presses, releases, mousePos);
    }
};

#endif //INPUTDATA_HPP

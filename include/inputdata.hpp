#ifndef INPUTDATA_HPP
#define INPUTDATA_HPP

#include <vector>

struct InputData {
    std::vector<int> presses; //Filled with whatever keyes that was pressed a tick
    std::vector<int> releases; //Filled with whatever keyes that was released a tick

    template<class Archive>
    void serialize(Archive& ar) {
        ar(presses, releases);
    }
};

#endif //INPUTDATA_HPP

#ifndef INPUTDATA_HPP
#define INPUTDATA_HPP

#include <queue>

struct InputData {
    std::queue<int> presses; //Filled with whatever keyes that was pressed a tick
    std::queue<int> releases; //Filled with whatever keyes that was released a tick

    template<class Archive>
    void serialize(Archive& ar) {
        ar(presses, releases);
    }
};

#endif //INPUTDATA_HPP

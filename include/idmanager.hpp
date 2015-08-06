#ifndef IDMANAGER_H
#define IDMANAGER_H
#include <stack>

typedef unsigned int ID;

/**
	This will cause some bugs if program creates more Id's than unsigned int can handle
**/
class IdManager {
private:
    std::stack<ID> freeIds;

public:
    static constexpr ID MAX_IDS = 2 << 24;
    IdManager();
    unsigned int acquireId();
    void releaseId(ID id);
};

#endif //IDMANAGER_H

#ifndef IDMANAGER_H
#define IDMANAGER_H
#include <stack>
#include <climits>

using std::stack;

typedef unsigned int ID;

/**
	This will cause some bugs if program creates more Id's than unsigned int can handle
**/
class IdManager {
private:
    static constexpr ID MAX_IDS = 65536;
    stack<ID> freeIds;

public:
    IdManager();
    unsigned int acquireId();
    void releaseId(ID id);
};

#endif //IDMANAGER_H

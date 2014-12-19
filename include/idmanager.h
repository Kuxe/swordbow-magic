#ifndef IDMANAGER_H
#define IDMANAGER_H

/**
	This will cause some bugs if program creates more Id's than unsigned int can handle
**/
class IdManager {
 public:
		unsigned long long int freeId = 0;
		unsigned long long int* getId();
};

#endif //IDMANAGER_H
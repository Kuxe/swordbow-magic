#include "idmanager.hpp"
#include <climits>
#include <iostream>

using namespace std;

unsigned long long int* IdManager::getId() {
	if(freeId > ULLONG_MAX - 10) {
		cout << "RUNNING OUT OF AVAILABLE IDS, BAD THINGS ARE ABOUT TO HAPPEN!" << endl;
		cout << "check IdManager-implementation" << endl;
	}
	return new auto(freeId++);
}

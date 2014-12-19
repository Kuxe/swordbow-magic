#ifndef SYSTEMMANGER_H
#define SYSTEMMANGER_H

#include <forward_list>
#include <unordered_map>

using namespace std;

class ISystem;

class SystemManager {
 public:
	forward_list<ISystem*> systems;
	unordered_map<string, ISystem*> systemByIdentifiers;
	void add(ISystem* system);
	void remove(ISystem* system);
	void update();
	ISystem* getSystem(string identifier);
};

#endif //SYSTEMMANGER_H
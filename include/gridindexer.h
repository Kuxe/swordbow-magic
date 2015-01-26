#ifndef GRIDINDEXER_H
#define GRIDINDEXER_H

#include <unordered_map>
#include <set>

using namespace std;

class MoveComponent;
class ComponentManager;

class GridIndexer {
 public:

struct CellIndex {
	unsigned char xcell;
	unsigned char ycell;
};

//Each cell has some ids associated with movecomponents
//(which are getted from componentmanager)
struct Cell {
	set<unsigned long long int*> ids;
};

	static constexpr unsigned char SIZE = 255;
	static constexpr unsigned char CELL_SIDE = 20;

	set<unsigned long long int*> ids;
	unordered_map<unsigned long long int*, CellIndex> cellIndicies;
	
 	Cell cells[SIZE][SIZE];
 	ComponentManager* const componentManager;

	GridIndexer(ComponentManager* componentManager);

	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void index(unsigned long long int* id);
	void update();
	set<unsigned long long int*> getCell(unsigned char x, unsigned char y) const;
	set<unsigned long long int*> getCell(unsigned long long int* id) const;
};

#endif //GRIDINDEXER_H
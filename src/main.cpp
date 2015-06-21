#include "server.hpp"
#include "componentmanager.hpp"
#include "idmanager.hpp"
#include <iostream>

constexpr void printGeneralInfo() {
	using std::cout;
	using std::endl;

	//Print some worst-case memory statistics, assuming
	//all entities have all components
	//(which isn't the case)
	uint64_t bytePerMegabyte = 1048576;
	uint64_t entityByteSize = ComponentManager::sizePerEntity();
	uint64_t allEntitiesByteSize = IdManager::MAX_IDS * entityByteSize;
	uint64_t allEntitiesMegabyteSize = allEntitiesByteSize / bytePerMegabyte;
	cout << "There can be at most " << IdManager::MAX_IDS << " ids";
	cout << " each of size " << entityByteSize << "bytes summing";
	cout << " up to " << allEntitiesByteSize;
	cout << "bytes (" << allEntitiesMegabyteSize;
	cout << "MB)" << endl;
}

int main(int argc, char** argv) {
	printGeneralInfo();
	Server server(argc, argv);
	server.run();
	return 0;
}

#include "server.hpp"
#include "client.hpp"

int main(int argc, char** argv) {

	//If first argument after program in terminal is client, ie:
	//@kuxe: ./swordbow-magic client
	//then launch a client, otherwise launch the server
	if(argc > 1 && strcmp(argv[1], "client") == 0) {
		Client client(argc, argv);
		client.run();
	} else {
		Server server(argc, argv);
		server.run();
	}
	return 0;
}

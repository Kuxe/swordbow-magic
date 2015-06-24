#include "server.hpp"
#include "client.hpp"
#include <thread>

int main(int argc, char** argv) {

	//If first argument after program in terminal is client, ie:
	//@kuxe: ./swordbow-magic client
	//then launch a client, otherwise launch the server
	if(argc > 1 && strcmp(argv[1], "client") == 0) {
		Client client(argc, argv);
		client.run();
	} else if (argc > 1 && strcmp(argv[1], "server") == 0) {
		Server server(argc, argv);
		server.run();
	} else if(argc > 1 && strcmp(argv[1], "threads") == 0) {
		Server server(argc, argv);
		Client client(argc, argv);
		client.connect(&server);

		std::thread serverThread(&Server::run, &server);
		client.run(); //It seems that SDL must be on main thread

		serverThread.join();
	} else {
		Server server(argc, argv);
		Client client(argc, argv);
		client.connect(&server);

		while(server.running && client.running) {
			server.step();
			client.step();
		}
	}
	return 0;
}

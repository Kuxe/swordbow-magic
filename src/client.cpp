#include "client.hpp"

/** For logging **/
#include "logger.hpp"

/** For network **/
#include "messagetypes.hpp"
#include "packet.hpp"
#include "systemidentifiers.hpp"

#include "inputdata.hpp"

/** For parsing **/
#include <string>

Client::Client(int argc, char** argv) :
        packetManager("swordbow-magic"),
        sequence(1),
        systemManager(&componentManager, &deltaTime),
        renderer(argc, argv),
        textureBoundingBox(&componentManager, &renderer),
        textureHashGridSystem(&textureBoundingBox),
        cameraSystem(&renderer),
        renderSystem(&renderer, &textureHashGridSystem, &cameraSystem),
        clientDisconnectedState(this),
        clientReceiveInitialState(this),
        clientRunningState(this),
        clientState(&clientDisconnectedState) {

    Logger::log("Starting client", Log::INFO);

    //Default port
    short port = 47294;

    //Check if port is set
    for(int i = 0; i < argc; i++) {
		if(strcmp(argv[i], "port") == 0) {
			port = std::atoi(argv[i+1]);
		}
	}

    packetManager.open(port);

    systemManager.add(&textureHashGridSystem);
    systemManager.add(&renderSystem);
    systemManager.add(&cameraSystem);

    keepAlive.start();
    keepAlive.stop();
}

Client::~Client() {
    packetManager.close();
    Logger::log("Destroying client", Log::INFO);
}

void Client::connect(const IpAddress& server) {
    renderer.showOverlay(Image::CONNECT_OVERLAY, {"Connecting to server...", 150, renderer.getScreenHeight() / 2 - 10});

    //Start a thread that checks socket for any recieved data
    receiveThreadRunning = true;
    receiveThread = std::thread(&Client::receive, this);

    this->server = server;

    //Connect-request packet
    send<bool, MESSAGE_TYPE::CONNECT>(true);
    keepAlive.start();
    std::ostringstream oss;
    oss << "Sent connect request to " << server;
    Logger::log(oss, Log::INFO);
}

void Client::disconnect() {
    //receiveThreadRunning can be seen as "isConnected"
    //So we don't need to disconnect if we're already disconnected.
    if(receiveThreadRunning == true) {
        receiveThreadRunning = false;
        receiveThread.join();

        //Disconnect-request packet
        send<bool, MESSAGE_TYPE::DISCONNECT>(true);

        keepAlive.stop();

        std::ostringstream oss;
        oss << "Disconnected from " << server;
        Logger::log(oss, Log::INFO);

        //Indicate not connected to any server
        server = IpAddress(0, 0, 0, 0, 0);
    }
}

void Client::receive() {
    while(receiveThreadRunning) {
        clientState->receive();
    }
}

void Client::run() {
    while(running) {
        step();
    }
}

void Client::step() {
    clientState->step();
}

void Client::stop() {
    running = false;
    Logger::log("Stopping client...", Log::INFO);
}

int main(int argc, char** argv) {
    Logger::openLogfile("clientlog.txt");
    Logger::level = Log::ERROR;

    /** Begin by parsing passed program arguments **/
    for(int i = 0; i < argc; i++) {
        std::string command(argv[i]);
        size_t pos = command.rfind("--log=");
        if(pos != std::string::npos) {
            std::string logstr = command.substr(pos+6);

            if(!logstr.compare("VERBOSE")) {
                Logger::level = Log::VERBOSE;
            } else if(!logstr.compare("INFO")) {
                Logger::level = Log::INFO;
            } else if(!logstr.compare("WARNING")) {
                Logger::level = Log::WARNING;
            } else if(!logstr.compare("ERROR")) {
                Logger::level = Log::ERROR;
            } else {
                Logger::log("Not valid value for --log=<VERBOSE|INFO|WARNING|ERROR>", Log::ERROR);
                return -1;
            }
        }
    }

    IpAddress ipAddress = {127, 0, 0, 1, 47293};
    for(int i = 0; i < argc; i++) {
        std::string command(argv[i]);
        size_t pos = command.rfind("--ip=");
        if(pos != std::string::npos) {
            std::string ipstr = command.substr(pos+5);
            pos = ipstr.find_first_of(".");
            unsigned char a = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(".");
            unsigned char b = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(".");
            unsigned char c = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(".");
            unsigned char d = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(":");
            unsigned short port = 47293;
            if(pos != std::string::npos) {
                port = stoi(ipstr.substr(pos+1));
            }

            ipAddress = {a, b, c, d, port};
        }
    }

    /** All arguments are parsed, now start the client **/
    Client client(argc, argv);
    client.connect(ipAddress);
    client.run();
    client.disconnect();
    client.stop();
    Logger::closeLogfile();
    return 0;
}








//

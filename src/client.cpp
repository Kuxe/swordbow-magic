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
#include "args.hxx"

Client::Client(bool fullscreenFlag, bool vsyncFlag, unsigned short port) :
        packetManager("swordbow-magic"),
        sequence(1),
        systemManager(&componentManager, &deltaTime),
        renderer(fullscreenFlag, vsyncFlag),
        textureBoundingBox(&componentManager, &renderer),
        textureHashGridSystem(&textureBoundingBox),
        cameraSystem(&renderer),
        renderSystem(&renderer, &textureHashGridSystem, &cameraSystem),
        clientDisconnectedState(this),
        clientReceiveInitialState(this),
        clientRunningState(this),
        clientState(&clientDisconnectedState) {

    Logger::info("Starting client");
    systemManager.add(&textureHashGridSystem);
    systemManager.add(&renderSystem);
    systemManager.add(&cameraSystem);

    packetManager.open(port);
    keepAlive.start();
    keepAlive.stop();
}

Client::~Client() {
    Logger::info("Destroying client");
    packetManager.close();
}

void Client::connect(const IpAddress& server) {
    renderer.showOverlay(Image::CONNECT_OVERLAY, {"Connecting to server...", 150, renderer.getScreenHeight() / 2 - 10});

    //Start a thread that checks socket for any recieved data
    receiveThreadRunning = true;
    receiveThread = std::thread(&Client::receive, this);

    this->server = server;

    //Connect-request packet
    send<bool, MESSAGE_TYPE::CONNECT_TO_SERVER>(true);
    keepAlive.start();
    std::ostringstream oss;
    oss << "Sent connect request to " << server;
    Logger::log(oss, Logger::INFO);
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
        Logger::info(oss);

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
    Logger::info("Stopping client...");
}

int main(int argc, char** argv) {
    /** Parse args (using nifty library by Taylor C. Richberger https://github.com/Taywee/args)**/
    args::ArgumentParser parser("swordbow-magic-client.", "swordbow-magic-client says bye!.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Flag fullscreenFlag(parser, "fullscreen", "Enable fullscreen", {"fullscreen"});
    args::Flag vsyncFlag(parser, "vsync", "Enable vsync", {"vsync"});
    args::ValueFlag<std::string> ipFlag(parser, "address", "Connects client to server at address", {"ip"});
    args::ValueFlag<unsigned short> portFlag(parser, "port", "Set port of client", {"port"});

    std::unordered_map<std::string, Log> map{
        {"VERBOSE", 1},
        {"INFO", 2},
        {"WARNING", 3},
        {"ERROR", 4},
    };

    args::MapFlag<std::string, uint8_t> logflag(parser, "VERBOSE|INFO|WARNING|ERROR", "Set logging level", {"log"}, map);

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return -1;
    }

    if(logflag) {
        Logger::openLogfile("serverlog.txt");
        Logger::enable();
        Logger::level = args::get(logflag);
    }

    IpAddress ipAddress = {127, 0, 0, 1, 47293};

    if(ipFlag) {
        std::string ipstr = args::get(ipFlag);
        size_t pos = ipstr.find_first_of(".");
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

    unsigned short port = 47294;
    if(portFlag) port = args::get(portFlag);
    bool fullscreen = false;
    if(fullscreenFlag) fullscreen = args::get(fullscreenFlag);
    bool vsync = false;
    if(vsyncFlag) vsync = args::get(vsyncFlag);

    /** All arguments are parsed, now start the client **/
    Client client(fullscreen, vsync, port);
    client.connect(ipAddress);
    client.run();
    client.disconnect();
    client.stop();
    Logger::closeLogfile();
    return 0;
}








//

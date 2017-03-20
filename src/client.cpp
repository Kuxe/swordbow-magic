#include "client.hpp"
#include <thread>

/** Graphics **/
#include "lowpolyadaptor.hpp"

/** Sound **/
#include "oggadaptor.hpp"

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

Client::Client(IRenderer* const renderer, ISoundEngine* const soundEngine, unsigned short port) :
        renderer(renderer),
        soundEngine(soundEngine),
        packetManager("swordbow-magic"),
        sequence(1),
        systemManager(&componentManager, &deltaTime),
        textureBoundingBox(&componentManager, renderer),
        textureHashGridSystem(&textureBoundingBox),
        cameraSystem(renderer),
        renderSystem(renderer, &textureHashGridSystem, &cameraSystem, &deltaTime),
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
    renderer->showOverlay(Image::CONNECT_OVERLAY, {"Connecting to server...", glm::ivec2{150, renderer->getWindowResolution().y / 2 - 10}, glm::vec3(1.0)});

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
        {
            std::ostringstream oss;
            oss << "Disconnecting from " << server << "...";
            Logger::info(oss);
        }
        receiveThreadRunning = false;
        Logger::verbose("Joining recieveThread in Client::disconnect()...");
        receiveThread.join();
        Logger::verbose("Joined recieveThread in Client::disconnect()");

        //Disconnect-request packet
        send<bool, MESSAGE_TYPE::DISCONNECT>(true);

        keepAlive.stop();

        std::ostringstream oss;
        oss << "Disconnected from " << server;
        Logger::info(oss);

        //Indicate not connected to any server
        server = IpAddress(0, 0, 0, 0, 0);
    } else {
        Logger::warning("Not connected to any server so could not disconnect");
    }
}

void Client::receive() {
    Logger::info("Client is about to let client state receive packets");
    while(receiveThreadRunning) {
        clientState->receive();
    }
     Logger::info("Receive thread no longer running");
}

void Client::run() {
    //Start client in a thread
    while(running) step();
}

void Client::step() {
    clientState->step();
}

void Client::stop() {
    running = false;
    Logger::info("Stopping client...");
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/******FIXME: liblowpoly3d.dll not copied into build so exe cant find it  *********/
/******(double click swordbow-magic-client.exe in explorer)************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/

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
        Logger::openLogfile("clientlog.txt");
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

    /** Adaptor classes are stored on stack here. If I ever want to swap out
        graphics or sound, or some library, it should be as simple as writing
        a new adaptor class and replacing current one here with the new adaptor class
        without bothering with other swordbow-magic code **/
    LowpolyAdaptor lowpolyAdaptor(fullscreen, vsync);
    OggAdaptor oggAdaptor;
    Client client(&lowpolyAdaptor, &oggAdaptor, port);
    client.connect(ipAddress);
    std::thread clientThread(&Client::run, &client); //Run game in a thread
    lowpolyAdaptor.run(); //Blocked in here until lowpoly3d renderer terminates
    client.disconnect();
    client.stop();
    clientThread.join();
    Logger::closeLogfile();
    return 0;
}

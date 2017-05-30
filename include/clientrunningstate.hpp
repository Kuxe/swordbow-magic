#ifndef CLIENTRUNNINGSTATE_HPP
#define CLIENTRUNNINGSTATE_HPP

#include "iclientstate.hpp"
#include "messagetypes.hpp"
#include "ipaddress.hpp"

class Client;
class ClientRunningState : public IClientState {
private:
	Client* client;

	std::vector<int> presses; //contains what keystrokes have been made (cleared on every gameloop)
    std::vector<int> releases; //contains what keyreleases have been made (cleared on every gameloop)
    glm::vec2 mousePos;
    bool mouseIsMoving = false; //A InputData packet will be sent if this is true
public:
	ClientRunningState(Client* client);
	void step();
	
	void changeState(IClientState* state);
	void onChange(ClientDisconnectedState* state);
	void onChange(ClientReceiveInitialState* state);
	void onChange(ClientRunningState* state);

	/** Methods handling various packets**/
	std::string name() const override;
	void greet(IPacket* packet) override;
	void handle(const OutdatedData* data) override;
	void handle(const ServerReplyToConnectData* data) override;
	void handle(const MoveComponentsDiffData* data) override;
	void handle(const RenderComponentsDiffData* data) override;
	void handle(const PlaySoundData* data) override;
	void handle(const RegisterIdToSystemData* data) override;
	void handle(const RemoveIdData* data) override;
	void handle(const RemoveIdFromSystemData* data) override;
	void handle(const RemoveIdFromSystemsData* data) override;
	void handle(const ActivateIdData* data) override;
	void handle(const KeepAliveData* data) override;

	void onEvent(const KeyEvent& evt);
	void onEvent(const MouseEvent& evt);
};

#endif //CLIENTRUNNINGSTATE_HPP
#ifndef ICLIENTSTATE_HPP
#define ICLIENTSTATE_HPP

#include "events.hpp"
#include "packethandler.hpp"

class ClientDisconnectedState;
class ClientReceiveInitialState;
class ClientRunningState;

/** State-pattern, interface of the different states a client can be in **/
class IClientState : public PacketHandler {
public:

	virtual void step() = 0;

	//Visitor-pattern, visitable = visitor because all states can change to any other.
	//changeState = accept, onChange = visist
	//It should be noted that, for example: ClientDisconnectedState::onChange(ClientReceivingState*)
	//contains code that happens when ClientReceivingState changes to ClientDisconnectedState
	//and not vice versa.
	//That is, ClientDisconnectedState changing to ClientReceivingState is handled in
	//ClientReceivingState::onChange(ClientDisconnectedState).
	//Think of it like "I just got from the argument to this, what should happen?"
	virtual void changeState(IClientState* state) = 0;
	virtual void onChange(ClientDisconnectedState* state) = 0;
	virtual void onChange(ClientReceiveInitialState* state) = 0;
	virtual void onChange(ClientRunningState* state) = 0;

	//Clients must explicitly deal with any packet they may receive
	//(including just discarding the packet)
	virtual void handle(IPacket* packet) = 0;

	virtual void onEvent(const KeyEvent& evt) = 0;
	virtual void onEvent(const MouseEvent& evt) = 0;
};

#endif //ICLIENTSTATE_HPP
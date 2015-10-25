#ifndef ICLIENTSTATE_HPP
#define ICLIENTSTATE_HPP

class ClientDisconnectedState;
class ClientReceiveInitialState;
class ClientRunningState;

/** State-pattern, interface of the different states a client can be in **/
class IClientState {
public:

	virtual void step() = 0;
	virtual void receive() = 0;

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
};

#endif //ICLIENTSTATE_HPP
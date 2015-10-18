#ifndef ICLIENTSTATE_HPP
#define ICLIENTSTATE_HPP

/** State-pattern, interface of the different states a client can be in **/
class IClientState {
public:
	virtual void step() = 0;
	virtual void receive() = 0;
};

#endif //ICLIENTSTATE_HPP
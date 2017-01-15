#ifndef OGGADAPTOR_HPP
#define OGGADAPTOR_HPP

#include "isoundengine.hpp"

class OggAdaptor : public ISoundEngine {
private:
	//TODO: Create member from 3rd party soundengine, use this member in order to get some functionality in methods

public:
	void playMusic(Music::Identifier music) const;
    void stopMusic(Music::Identifier music) const;
    void playSound(SoundData& scSound) const;
    void stopSound(SoundData& scSound) const;
};


#endif //OGGADDAPTOR_HPP
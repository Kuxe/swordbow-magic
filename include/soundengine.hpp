#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <unordered_map>
#include "sounddata.hpp"
#include "soundidentifiers.hpp"
#include "musicidentifiers.hpp"

//TODO: Use lowpoly3d
/** Should probably make this into an interface which can be realized by some 3rd party library
	Ideally, let MyThirdPartyClass be a class which can play various music and sounds.
	Then another class MyThirdPartyClassSoundEngine may realize an ISoundEngine interface.
	The MyThirdPartyClassSoundEngine has a member MyThirdPartyClass, whose methods are used
	in methods of MyThirdPartyClassSoundEngine. In this way, SoundEngine in swordbow-magic
	is decoupled from library which is nice. swordbow-magic only knows about ISoundEngine,
	so whenever I want to switch sound library I just need to implement a new
	MyOtherThirdPartyClassSoundEngine, without bothering with other swordbow-magic code.
	I think this is the adaptor-pattern. **/

typedef unsigned int ID;

class SoundEngine {
private:

public:
    SoundEngine();
    ~SoundEngine();
    void playMusic(Music::Identifier music) const;
    void stopMusic(Music::Identifier music) const;
    void playSound(SoundData& scSound) const;
    void stopSound(SoundData& scSound) const;
};

#endif //SOUNDSYSTEM_H

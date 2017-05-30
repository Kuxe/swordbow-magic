#ifndef ISOUNDENGINE_HPP
#define ISOUNDENGINE_HPP

#include <unordered_map>
#include "sounddata.hpp"
#include "soundidentifiers.hpp"
#include "musicidentifiers.hpp"

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

struct ISoundEngine {
    virtual ~ISoundEngine() { }
    virtual void playMusic(Music::Identifier music) const = 0;
    virtual void stopMusic(Music::Identifier music) const = 0;
    virtual void playSound(SoundData& scSound) const = 0;
    virtual void stopSound(SoundData& scSound) const = 0;
};

#endif //ISOUNDENGINE_HPP

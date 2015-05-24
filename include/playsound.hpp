#ifndef PLAYSOUND_HPP
#define PLAYSOUND_HPP

#include "icommand.hpp"
#include <string>
#include "soundcomponent.hpp"

using std::string;

class SoundSystem;
class PlaySound : public ICommand {
private:
    SoundSystem* soundSystem;
    SoundComponent::Sound sound;
public:
    PlaySound(SoundSystem* soundSystem, const SoundComponent::Sound& soundPath);
    void execute();
};

#endif //PLAYSOUND_HPP

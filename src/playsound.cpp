#include "playsound.hpp"
#include "soundsystem.hpp"

PlaySound::PlaySound(SoundSystem* soundSystem, const SoundComponent::Sound& sound) :
    soundSystem(soundSystem),
    sound(sound) { }

void PlaySound::execute() {
    soundSystem->playSound(sound);
}

#ifndef OVERLAY_HPP
#define OVERLAY_HPP

#include "imageidentifiers.hpp"
#include "timer.hpp"
#include "text.hpp"

/** Represents an graphical overlay that covers the whole window **/
class Overlay {
private:
	Timer fadeInTimer;
	float fadeInSeconds = 1;
	Timer fadeOutTimer;
	float fadeOutSeconds = 0;
	const Text text;
public:
	const Image::Identifier identifier;
	Overlay(Image::Identifier identifier, const Text text = {"", 0, 0});
	void show();
	void hide();
	void fadeIn(float seconds);
	void fadeOut(float seconds);
	float getTransparency() const;
	const Text& getText() const;
};

#endif //OVERLAY_HPP
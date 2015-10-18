#include "overlay.hpp"
#include "logger.hpp"

Overlay::Overlay(Image::Identifier identifier, const Text text) : 
	identifier(identifier),
	text(text) { }

void Overlay::show() {
	fadeIn(0);
}

void Overlay::hide() {
	fadeOut(0);
}

void Overlay::fadeIn(float seconds) {
	fadeInSeconds = seconds;
	fadeInTimer.start();
}

void Overlay::fadeOut(float seconds) {
	fadeOutSeconds = seconds;
	fadeOutTimer.start();
}

float Overlay::getTransparency() const {

	float fadeInPercentage = 1.0f;
	if(fadeInSeconds != 0.0f) {
		fadeInPercentage = fadeInTimer.elapsed() / fadeInSeconds;
		fadeInPercentage = fadeInPercentage >= 1 ? 1 : fadeInPercentage;
	}

	float fadeOutPercentage = 0.0f;
	if(fadeOutSeconds != 0.0f) {
		fadeOutPercentage = fadeOutTimer.elapsed() / fadeOutSeconds;
		fadeOutPercentage = fadeOutPercentage >= 1 ? 1 : fadeOutPercentage;
	}

	return 1 + fadeInPercentage * (fadeOutPercentage - 1);
}

const Text& Overlay::getText() const {
	return text;
}
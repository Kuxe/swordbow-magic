#include "renderer.hpp"

/** For logging **/
#include "logger.hpp"
#include <ostream>

/** For strmcp **/
#include <cstring>



Renderer::Renderer(bool fullscreen, bool vsync) {

}

Renderer::~Renderer() {

}

void Renderer::renderOnlyOverlays() {

}

void Renderer::printText(const Text& text) {

}

void Renderer::renderTexts() {

}

const std::unordered_map<Image::Identifier, TextureData, std::hash<int>>& Renderer::getTextureDatas() const {

}

void Renderer::showOverlay(Image::Identifier identifier, Text text) {

}

void Renderer::hideOverlay(Image::Identifier identifier) {

}

void Renderer::fadeInOverlay(Image::Identifier identifier, float seconds, Text text) {

}


void Renderer::fadeOutOverlay(Image::Identifier identifier, float seconds) {

}

void Renderer::renderOverlays() {
    
}

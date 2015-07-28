#include "renderdiffsystem.hpp"
#include "componentmanager.hpp"

void RenderDiffSystem::add(ID id) {
    ids.insert(id);
}
void RenderDiffSystem::remove(ID id) {
    ids.erase(id);
}
void RenderDiffSystem::update() {
    componentManager->renderComponentsDiff.clear();
    for(auto id : ids) {
        componentManager->renderComponentsDiff.insert({id, componentManager->renderComponents.at(id)});
    }
}
unsigned int RenderDiffSystem::count() const {
    return ids.size();
}
const System::Identifier RenderDiffSystem::getIdentifier() const {
    return System::RENDERDIFF;
}
void RenderDiffSystem::activateId(ID id) {
    add(id);
}

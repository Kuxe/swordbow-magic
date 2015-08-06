#include "renderdiffsystem.hpp"
#include "componentmanager.hpp"

void RenderDiffSystem::add(ID id) {
    ids.insert(id);
}
void RenderDiffSystem::remove(ID id) {
    ids.erase(id);
}
void RenderDiffSystem::update() {
	ids.clear();
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

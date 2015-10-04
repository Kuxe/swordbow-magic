#include "renderdiffsystem.hpp"
#include "componentmanager.hpp"

void RenderDiffSystem::add(ID id) {
    ids.insert(id);
}
void RenderDiffSystem::remove(ID id) {
    ids.erase(id);
}
void RenderDiffSystem::update() {
	//ids.clear();
}
unsigned int RenderDiffSystem::count() const {
    return ids.size();
}

void RenderDiffSystem::clear() {
	ids.clear();
}

const System::Identifier RenderDiffSystem::getIdentifier() const {
    return System::RENDERDIFF;
}
bool RenderDiffSystem::activateId(ID id) {
    add(id);
    return true;
}

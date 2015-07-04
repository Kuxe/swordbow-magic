#include "renderdiffsystem.hpp"
#include "componentmanager.hpp"

void RenderDiffSystem::add(ID id) {
    ids.push(id);
}
void RenderDiffSystem::remove(ID id) {

}
void RenderDiffSystem::update() {
    componentManager->renderComponentsDiff.clear();
    while(!ids.empty()) {
        const auto id = ids.front(); ids.pop();
        componentManager->renderComponentsDiff.insert({id, componentManager->renderComponents.at(id)});
    }
}
unsigned int RenderDiffSystem::count() const {
    return ids.size();
}
const string RenderDiffSystem::getIdentifier() const {
    return "RenderDiffSystem";
}
void RenderDiffSystem::activateId(ID id) {
    add(id);
}

#include "sizeboundingbox.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "sizecomponent.h"

SizeBoundingBox::SizeBoundingBox(ComponentManager* componentManager):
    BoundingBox(componentManager) {}

unsigned int SizeBoundingBox::getX(ID id) {
    return  componentManager->moveComponents.at(id)->xpos;
}
unsigned int SizeBoundingBox::getY(ID id) {
    return  componentManager->moveComponents.at(id)->ypos;
}
unsigned int SizeBoundingBox::getOldX(ID id) {
    return  componentManager->moveComponents.at(id)->oldXpos;
}
unsigned int SizeBoundingBox::getOldY(ID id) {
    return  componentManager->moveComponents.at(id)->oldYpos;
}
unsigned int SizeBoundingBox::getW(ID id) {
    return componentManager->sizeComponents.at(id)->width;
}
unsigned int SizeBoundingBox::getH(ID id) {
    return componentManager->sizeComponents.at(id)->height;
}

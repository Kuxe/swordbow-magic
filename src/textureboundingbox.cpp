#include "textureboundingbox.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "rendercomponent.h"

TextureBoundingBox::TextureBoundingBox(ComponentManager* componentManager):
    BoundingBox(componentManager) {}

unsigned int TextureBoundingBox::getX(ID id) {
    return  componentManager->moveComponents.at(id)->xpos +
            componentManager->renderComponents.at(id)->xoffset;
}
unsigned int TextureBoundingBox::getY(ID id) {
    return  componentManager->moveComponents.at(id)->ypos +
            componentManager->renderComponents.at(id)->yoffset;
}
unsigned int TextureBoundingBox::getOldX(ID id) {
    return  componentManager->moveComponents.at(id)->oldXpos +
            componentManager->renderComponents.at(id)->xoffset;
}
unsigned int TextureBoundingBox::getOldY(ID id) {
    return  componentManager->moveComponents.at(id)->oldYpos +
            componentManager->renderComponents.at(id)->yoffset;
}
unsigned int TextureBoundingBox::getW(ID id) {
    return componentManager->renderComponents.at(id)->textureData.width;
}
unsigned int TextureBoundingBox::getH(ID id) {
    return componentManager->renderComponents.at(id)->textureData.height;
}
